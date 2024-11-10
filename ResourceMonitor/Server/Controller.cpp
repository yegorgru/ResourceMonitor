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
    
    LOG::Debug(LOG::composeMessage("Got command-line arguments:", "Port:", port, "Threads count:", threadCount));

    mServer.start(port, threadCount);

    std::string command;
    while (true) {
        LOG::SyncPrintLine("Enter command:", std::cout);
        std::cin >> command;
        if (command == "exit") {
            LOG::SyncPrintLine("Stopping server...", std::cout);
            mServer.stop();
            LOG::Info("Exiting application");
            return;
        }
        else {
            LOG::SyncPrintLine("Unknown command", std::cout);
            LOG::Debug(LOG::composeMessage("Unknown command entered:", command));
        }
    }
}

} // namespace ResourceMonitorServer