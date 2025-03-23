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

void Controller::printHelpMessage() {
    LOG::SyncPrintLine("\nAvailable commands:", std::cout);
    LOG::SyncPrintLine("  help              - Display this help message", std::cout);
    LOG::SyncPrintLine("  exit              - Stop the server and exit the application", std::cout);
    LOG::SyncPrintLine("\nSupported HTTP endpoints:", std::cout);
    LOG::SyncPrintLine("  GET /<resource>/<count>/<ip>", std::cout);
    LOG::SyncPrintLine("    - Retrieve resource monitoring data", std::cout);
    LOG::SyncPrintLine("    - <resource>: basic_info, cpu, memory, disks, network", std::cout);
    LOG::SyncPrintLine("    - <count>: number of measurements to retrieve", std::cout);
    LOG::SyncPrintLine("    - <ip>: target machine IP address", std::cout);
    LOG::SyncPrintLine("  PUT /<resource>/<ip>", std::cout);
    LOG::SyncPrintLine("    - Store resource monitoring data", std::cout);
    LOG::SyncPrintLine("    - <resource>: basic_info, cpu, memory, disks, network", std::cout);
    LOG::SyncPrintLine("    - <ip>: source machine IP address\n", std::cout);
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
    const auto& dbName = mArgumentParser.getDbName();
    auto dbPort = mArgumentParser.getDbPort();
    
    LOG::Debug(LOG::composeMessage("Got command-line arguments:", "Port:", port, "Threads count:", threadCount));
    LOG::Debug(LOG::composeMessage("Got command-line db arguments:", "Port:", dbPort, "Db name:", dbName));

    DatabaseManager::Init(dbName, dbPort);
    LOG::Debug("Database is initialized");

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
        else if (command == "help") {
            printHelpMessage();
        }
        else {
            LOG::SyncPrintLine("Unknown command. Type 'help' for available commands.", std::cout);
            LOG::Debug(LOG::composeMessage("Unknown command entered:", command));
        }
    }
}

} // namespace ResourceMonitorServer