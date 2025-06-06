#include <thread>
#include <mutex>
#include <memory>
#include <iostream>
#include <map>

#include "Factory.h"
#include "Controller.h"

int main(int argc, char* argv[])
{
    using namespace ResourceMonitorClient;

    try {
        auto client = Http::createNetworkClient();
        Controller controller(std::move(client));
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
};

