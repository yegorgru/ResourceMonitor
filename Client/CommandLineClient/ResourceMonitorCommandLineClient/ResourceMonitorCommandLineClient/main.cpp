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

        auto request_one = client.createRequest(1);

        request_one->setHost("localhost");
        request_one->setUri("/index.html");
        request_one->setPort(3333);

        request_one->execute();

        client.close();
    }
    catch (boost::system::system_error& e) {
        std::cout << "Error occured! Error code = " << e.code() << ". Message: " << e.what();

        return e.code().value();
    }

    return 0;
};

