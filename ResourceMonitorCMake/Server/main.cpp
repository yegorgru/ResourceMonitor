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
        IoService::Init();
        Server server(IoService::Get().getIoService());
        Controller controller(server);
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
}
