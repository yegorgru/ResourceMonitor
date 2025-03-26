#include <thread>
#include <iostream>

#include "Server.h"
#include "Controller.h"
#include "IoService.h"

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
        PRINT::PrintLine(PRINT::composeMessage("Boost error occured! Error code =", e.code(), ". Message:", e.what()), std::cout);
    }
    catch (std::exception& e) {
        PRINT::PrintLine(PRINT::composeMessage("Std error occured! Message:", e.what()), std::cout);
    }
    catch (...) {
        PRINT::PrintLine("Unknown error occured!", std::cout);
    }

    return 0;
}
