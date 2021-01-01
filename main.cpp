#include <asio.hpp>

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <memory>
#include <utility>
#include "testEchoServerClass.h"

#include "server.hpp"

int testCaseNumber = 1;

/*
request_handler 处理数据，状态写到request里
*/
int main(int argc, char* argv[])
{
    std::fstream f;
    if(testCaseNumber ==0)
    {
        try
        {
            asio::io_context io_context;
            AsyncEchoServer s(io_context, 8899);
            io_context.run();
        }
        catch (std::exception& e)
        {
            std::cerr << "Exception: " << e.what() << "\n";
        }
    }
    else if (testCaseNumber == 1)
    {
        try
        {
            //asio::io_context io_context_;
            http::server::server s("127.0.0.1", "9909", "./");
            s.run();
        }
        catch (std::exception& e)
        {
            std::cerr << "exception: " << e.what() << "\n";
        }
    }


    return 0;
}