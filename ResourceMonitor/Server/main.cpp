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
}
