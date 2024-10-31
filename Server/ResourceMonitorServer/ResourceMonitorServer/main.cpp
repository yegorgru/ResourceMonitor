#include <thread>
#include <iostream>

#include "Server.h"

const unsigned int DEFAULT_THREAD_POOL_SIZE = 2;

int main()
{
    using namespace ResourceMonitorServer;

    Server::Port portNum = 3333;

    try {
        Server srv(portNum);

        unsigned int threadPoolSize = std::thread::hardware_concurrency();

        if (threadPoolSize == 0) {
            threadPoolSize = DEFAULT_THREAD_POOL_SIZE;
        }

        srv.start(threadPoolSize);

        std::this_thread::sleep_for(std::chrono::seconds(2000));

        srv.stop();
    }
    catch (boost::system::system_error& e) {
        std::cout << "Error occured! Error code = " << e.code() << ". Message: " << e.what();
    }

    return 0;
}
