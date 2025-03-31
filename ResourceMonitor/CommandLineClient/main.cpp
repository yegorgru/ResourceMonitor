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
    catch (const boost::system::system_error& e) {
        Print::PrintLine(Print::composeMessage("Boost error occured! Error code =", e.code(), ". Message:", e.what()));
    }
    catch (const std::exception& e) {
        Print::PrintLine(Print::composeMessage("Std error occured! Message:", e.what()));
    }
    catch (...) {
        Print::PrintLine("Unknown error occured!");
    }

    return 0;
};

