#include <thread>
#include <mutex>
#include <memory>
#include <iostream>
#include <map>

#include "Client.h"

int main()
{
    using namespace ResourceMonitorClient;
    try {
        Client client;
        client.emulateLongComputationOp(10, "127.0.0.1", 3333, 1);
        std::this_thread::sleep_for(std::chrono::seconds(10));
        client.close();
    }
    catch (boost::system::system_error& e) {
        std::cout << "Error occured! Error code = " << e.code() << ". Message: " << e.what();
        return e.code().value();
    }

    return 0;
};
