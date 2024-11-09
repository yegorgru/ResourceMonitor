#include "Controller.h"
#include "Log.h"

#include <iostream>

namespace ResourceMonitorServer {

Controller::Controller(Server& server)
    : mServer(server)
    , mIsValidState(true)
{
}

void Controller::init(int argc, char* argv[]) {
    mIsValidState = mArgumentParser.parseCommandLine(argc, argv);
}

void Controller::run() {
    if (!mIsValidState) {
        return;
    }
    const auto logFilename = mArgumentParser.getLogFilename();
    const auto logLevel = mArgumentParser.getLogLevel();
    if (logFilename == "") {
        LOG::initConsoleLogger(logLevel);
    }
    else {
        LOG::initFileLogger(logLevel, logFilename);
    }
    LOG::Debug("Logger is initialized");

    const auto port = mArgumentParser.getPort();
    const auto threadCount = mArgumentParser.getThreadCount();
    
    LOG::Debug(LOG::makeLogMessage("Got command-line arguments:", "Port:", port, "Threads count:", threadCount));

    mServer.start(port, threadCount);

    std::string command;
    while (true) {
        //std::cout << "> ";
        std::cin >> command;
        if (command == "exit") {
            std::cout << "Stopping server..." << std::endl;
            mServer.stop();
            LOG::Info("Exiting application");
            return;
        }
        else {
            std::cout << "Unknown command" << std::endl;
            LOG::Debug(LOG::makeLogMessage("Unknown command entered:", command));
        }
    }
}

} // namespace ResourceMonitorServer