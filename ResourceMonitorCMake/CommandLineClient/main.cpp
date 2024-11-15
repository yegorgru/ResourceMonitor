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
        LOG::SyncPrintLine(LOG::composeMessage("Boost error occured! Error code =", e.code(), ". Message:", e.what()), std::cout);
    }
    catch (std::exception& e) {
        LOG::SyncPrintLine(LOG::composeMessage("Std error occured! Message:", e.what()), std::cout);
    }
    catch (...) {
        LOG::SyncPrintLine("Unknown error occured!", std::cout);
    }

    return 0;
};

