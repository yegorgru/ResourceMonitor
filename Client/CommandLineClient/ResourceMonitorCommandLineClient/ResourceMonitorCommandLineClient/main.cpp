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

        /*auto request_two = client.createRequest(2);

        request_two->set_host("localhost");
        request_two->set_uri("/example.html");
        request_two->set_port(3333);

        request_two->execute();

        request_two->cancel();*/

        // Do nothing for 15 seconds, letting the
        // request complete.
        std::this_thread::sleep_for(std::chrono::seconds(15));

        // Closing the client and exiting the application.
        client.close();
    }
    catch (boost::system::system_error& e) {
        std::cout << "Error occured! Error code = " << e.code() << ". Message: " << e.what();

        return e.code().value();
    }

    return 0;
};

