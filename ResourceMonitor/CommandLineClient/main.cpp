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
        PRINT::PrintLine(PRINT::composeMessage("Boost error occured! Error code =", e.code(), ". Message:", e.what()), std::cout);
    }
    catch (std::exception& e) {
        PRINT::PrintLine(PRINT::composeMessage("Std error occured! Message:", e.what()), std::cout);
    }
    catch (...) {
        PRINT::PrintLine("Unknown error occured!", std::cout);
    }

    return 0;
};

