#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>
#include "asio.hpp"

using asio::ip::tcp;

/*
当类A被share_ptr管理，且在类A的成员函数里需要把当前类对象作为参数传给其他函数时，
就需要传递一个指向自身的share_ptr。
*/
class AsyncEchoServerSession
    : public std::enable_shared_from_this<AsyncEchoServerSession>
{
public:
    AsyncEchoServerSession(tcp::socket socket)
        : socket_(std::move(socket))
    {
    }

    void start()
    {
        do_read();
    }

private:
    void do_read()
    {
        auto self(shared_from_this());
        std::cout << self.use_count() << std::endl;
        socket_.async_read_some(asio::buffer(data_, max_length),
            [this, self](std::error_code ec, std::size_t length)
            {
                //in this example, as a fd, read && write are seq now
                std::cout << self.use_count() << std::endl;
                if (!ec)
                {
                    do_write(length);
                    std::cout << self.use_count() << std::endl;
                }
            });
        std::cout << self.use_count() << std::endl;
    }

    void do_write(std::size_t length)
    {
        auto self(shared_from_this());
        std::cout << self.use_count() << std::endl;
        asio::async_write(socket_, asio::buffer(data_, length),
            [this, self](std::error_code ec, std::size_t /*length*/)
            {
                if (!ec)
                {
                    std::cout << self.use_count() << std::endl;
                    do_read();
                    std::cout << self.use_count() << std::endl;
                }
            });
        std::cout << self.use_count() << std::endl;
    }

    tcp::socket socket_;
    enum { max_length = 1024 };
    char data_[max_length];
};

class AsyncEchoServer
{
public:
    AsyncEchoServer(asio::io_context& io_context, short port)
        : acceptor_(io_context, tcp::endpoint(tcp::v4(), port))
    {
        do_accept();
    }

private:
    void do_accept()
    {
        //设置acceptor接收一个客户端的回调
        acceptor_.async_accept(
            [this](std::error_code ec, tcp::socket socket)
            {
                if (!ec)
                {
                    std::make_shared<AsyncEchoServerSession>(std::move(socket))->start();
                }

                do_accept();
            });
    }

    tcp::acceptor acceptor_;
};


