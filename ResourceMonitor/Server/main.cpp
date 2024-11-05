#include <thread>
#include <iostream>

#include "Server.h"
#include "Controller.h"

#include "Log.h"

int main(int argc, char* argv[])
{
    using namespace ResourceMonitorServer;

    try {
        Server server;
        Controller controller(server);
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
}
