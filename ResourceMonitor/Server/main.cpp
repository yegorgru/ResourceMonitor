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
        std::cout << "Error occured! Error code = " << e.code() << ". Message: " << e.what();
    }

    return 0;
}
