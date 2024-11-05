#include <thread>
#include <mutex>
#include <memory>
#include <iostream>
#include <map>

#include "Client.h"
#include "Controller.h"

int main(int argc, char* argv[])
{
    using namespace ResourceMonitorClient;

    try {
        Client client;
        Controller controller(client);
        controller.init(argc, argv);
        controller.run();
    }
    catch (boost::system::system_error& e) {
        auto message = LOG::makeLogMessage("Boost error occured! Error code =", e.code(), ". Message:", e.what());
        std::cout << message << std::endl;
        LOG::Error(message);
    }
    catch (std::exception& e) {
        auto message = LOG::makeLogMessage("Std error occured! Message:", e.what());
        std::cout << message << std::endl;
        LOG::Error(message);
    }
    catch (...) {
        std::string message = "Unknown error occured!";
        std::cout << message << std::endl;
        LOG::Error(message);
    }

    return 0;
};

