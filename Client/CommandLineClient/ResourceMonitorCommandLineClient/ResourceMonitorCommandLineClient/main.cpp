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

        request_one->set_host("localhost");
        request_one->set_uri("/index.html");
        request_one->set_port(3333);

        request_one->execute();

        client.close();
    }
    catch (boost::system::system_error& e) {
        std::cout << "Error occured! Error code = " << e.code() << ". Message: " << e.what();

        return e.code().value();
    }

    return 0;
};

