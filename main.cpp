#include <asio.hpp>

#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>
#include "testEchoServerClass.h"

int testCaseNumber = 0;

int main(int argc, char* argv[])
{
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

    }


    return 0;
}