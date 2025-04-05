#include <thread>
#include <iostream>

#include "Factory.h"
#include "Controller.h"

#include "Log.h"

int main(int argc, char* argv[])
{
    using namespace ResourceMonitorServer;

    try {
        auto server = Http::createNetworkServer();
        Controller controller(std::move(server));
        controller.init(argc, argv);
        controller.run();
    }
    catch (const std::exception& e) {
        Print::PrintLine(Print::composeMessage("Std error occured! Message:", e.what()));
    }
    catch (...) {
        Print::PrintLine("Unknown error occured!");
    }

    return 0;
}
