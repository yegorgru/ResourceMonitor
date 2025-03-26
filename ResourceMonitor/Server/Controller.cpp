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
    PRINT::PrintLine("\nAvailable commands:", std::cout);
    PRINT::PrintLine("  help              - Display this help message", std::cout);
    PRINT::PrintLine("  config            - Enter configuration mode", std::cout);
    PRINT::PrintLine("  exit              - Stop the server and exit the application", std::cout);
}

void Controller::handleCommand(const std::string& command) {
    if (command == "exit") {
        PRINT::PrintLine("Stopping server...", std::cout);
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
            PRINT::PrintLine("Restarting server to apply configuration changes...", std::cout);
            mServer.stop();
            mServer.start(mConfig.getPort(), mConfig.getThreadCount());
            mConfig.resetServerRestartFlag();
            PRINT::PrintLine("Server restarted successfully", std::cout);
        }
    }
    else {
        PRINT::PrintLine("Unknown command. Type 'help' for available commands.", std::cout);
        LOG::Debug(PRINT::composeMessage("Unknown command entered:", command));
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
    
    LOG::Debug(PRINT::composeMessage("Got command-line arguments:", "Port:", port, "Threads count:", threadCount));
    LOG::Debug(PRINT::composeMessage("Got command-line db arguments:", "Port:", dbPort, "Db name:", dbName));

    DatabaseManager::Init(dbName, dbPort);
    LOG::Debug("Database is initialized");

    mServer.start(port, threadCount);

    std::string command;
    while (true) {
        PRINT::PrintLine("Enter command:", std::cout);
        std::cin >> command;
        handleCommand(command);
        if (command == "exit") {
            break;
        }
    }
}

} // namespace ResourceMonitorServer