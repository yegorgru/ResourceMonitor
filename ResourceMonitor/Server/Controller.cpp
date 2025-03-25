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
    mIsValidState = mConfig.parseCommandLine(argc, argv);
}

void Controller::printHelpMessage() {
    LOG::SyncPrintLine("\nAvailable commands:", std::cout);
    LOG::SyncPrintLine("  help              - Display this help message", std::cout);
    LOG::SyncPrintLine("  config            - Enter configuration mode", std::cout);
    LOG::SyncPrintLine("  exit              - Stop the server and exit the application", std::cout);
}

void Controller::handleCommand(const std::string& command) {
    if (command == "exit") {
        LOG::SyncPrintLine("Stopping server...", std::cout);
        mServer.stop();
        LOG::Info("Exiting application");
        return;
    }
    else if (command == "help") {
        printHelpMessage();
    }
    else if (command == "config") {
        mConfig.handleConfigCommand();
        if (mConfig.isServerRestartNeeded()) {
            LOG::SyncPrintLine("Restarting server to apply configuration changes...", std::cout);
            mServer.stop();
            mServer.start(mConfig.getPort(), mConfig.getThreadCount());
            mConfig.resetServerRestartFlag();
            LOG::SyncPrintLine("Server restarted successfully", std::cout);
        }
    }
    else {
        LOG::SyncPrintLine("Unknown command. Type 'help' for available commands.", std::cout);
        LOG::Debug(LOG::composeMessage("Unknown command entered:", command));
    }
}

void Controller::run() {
    if (!mIsValidState) {
        return;
    }

    const auto logFilename = mConfig.getLogFilename();
    const auto logLevel = mConfig.getLogLevel();
    if (logFilename == "") {
        LOG::initConsoleLogger(logLevel);
    }
    else {
        LOG::initFileLogger(logLevel, logFilename);
    }
    LOG::Debug("Logger is initialized");

    const auto port = mConfig.getPort();
    const auto threadCount = mConfig.getThreadCount();
    const auto& dbName = mConfig.getDbName();
    auto dbPort = mConfig.getDbPort();
    
    LOG::Debug(LOG::composeMessage("Got command-line arguments:", "Port:", port, "Threads count:", threadCount));
    LOG::Debug(LOG::composeMessage("Got command-line db arguments:", "Port:", dbPort, "Db name:", dbName));

    DatabaseManager::Init(dbName, dbPort);
    LOG::Debug("Database is initialized");

    mServer.start(port, threadCount);

    std::string command;
    while (true) {
        LOG::SyncPrintLine("Enter command:", std::cout);
        std::cin >> command;
        handleCommand(command);
        if (command == "exit") {
            break;
        }
    }
}

} // namespace ResourceMonitorServer