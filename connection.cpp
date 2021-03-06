//
// connection.cpp
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2020 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "connection.hpp"
#include <utility>
#include <vector>
#include "connection_manager.hpp"
#include "request_handler.hpp"

namespace http {
namespace server {

connection::connection(asio::ip::tcp::socket socket,
    connection_manager& manager, request_handler& handler)
  : socket_(std::move(socket)),
    connection_manager_(manager),
    request_handler_(handler)
{
}

void connection::start()
{
  do_read();
}

void connection::stop()
{
  socket_.close();
}

void connection::do_read()
{
  auto self(shared_from_this());
  socket_.async_read_some(asio::buffer(buffer_),
      [this, self](std::error_code ec, std::size_t bytes_transferred)
      {
        if (!ec)
        {
          request_parser::result_type result;
          std::tie(result, std::ignore) = request_parser_.parse(
              request_, buffer_.data(), buffer_.data() + bytes_transferred);

          if (result == request_parser::good)
          {
            request_handler_.handle_request(request_, reply_);
            do_write();
          }
          else if (result == request_parser::bad)
          {
            reply_ = reply::stock_reply(reply::bad_request);
            do_write();
          }
          else
          {
            do_read();
          }
        }
        else if (ec != asio::error::operation_aborted)
        {
          connection_manager_.stop(shared_from_this());
        }
      });
}

void connection::do_write()
{
  auto self(shared_from_this());
  auto buff = reply_.header_to_buffers();
  socket_.async_write_some(buff, [this, self](std::error_code ec, std::size_t len)
      {
          if (!ec)
          {
              do_write_some();
          }
          else
          {
              asio::error_code ignored_ec;
              socket_.shutdown(asio::ip::tcp::socket::shutdown_both, ignored_ec);
              if (ec != asio::error::operation_aborted)
                  connection_manager_.stop(shared_from_this());
          }
      });
}

void connection::do_write_some()
{
    auto self(shared_from_this());
    reply_.content = reply_.read_file_piece();
    if (reply_.content.empty())
        return;
    socket_.async_write_some(asio::buffer(reply_.content),
        [this, self](std::error_code ec, std::size_t len)
        {
            if (!ec)
            {
                do_write_some();
            }
            else
            {
                asio::error_code ignored_ec;
                socket_.shutdown(asio::ip::tcp::socket::shutdown_both, ignored_ec);
                if (ec != asio::error::operation_aborted)
                    connection_manager_.stop(shared_from_this());
            }
        });
}

} // namespace server
} // namespace http
