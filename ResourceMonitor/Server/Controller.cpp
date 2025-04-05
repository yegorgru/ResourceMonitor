#include "Controller.h"
#include "Log.h"
#include "Input.h"

#include <iostream>

namespace ResourceMonitorServer {

Controller::Controller(Http::ServerPtr&& server)
    : mServer(std::move(server))
    , mConfig(*mServer)
    , mIsValidState(true)
{
}

void Controller::init(int argc, char* argv[]) {
    mIsValidState = mConfig.parseCommandLine(argc, argv);
}

void Controller::printHelpMessage() {
    Print::PrintLine("\nAvailable commands:");
    Print::PrintLine("  help              - Display this help message");
    Print::PrintLine("  config            - Enter configuration mode");
    Print::PrintLine("  exit              - Stop the server and exit the application");
}

void Controller::handleCommand(const std::string& command) {
    if (command == "exit") {
        Print::PrintLine("Stopping server...");
        mServer->stop();
        Log::Info("Exiting application");
        return;
    }
    else if (command == "help") {
        printHelpMessage();
    }
    else if (command == "config") {
        mConfig.handleConfigCommand();
    }
    else {
        Print::PrintLine("Unknown command. Type 'help' for available commands.");
        Log::Debug(Print::composeMessage("Unknown command entered:", command));
    }
}

void Controller::run() {
    if (!mIsValidState) {
        return;
    }

    Print::initConsolePrinter();
    const auto logFilename = mConfig.getLogFilename();
    const auto logLevel = mConfig.getLogLevel();
    if (logFilename == "") {
        Log::initConsoleLogger(logLevel);
    }
    else {
        Log::initFileLogger(logLevel, logFilename);
    }

    Input::initConsoleReader();

    Log::Debug("Logger, printer and reader are initialized");

    const auto port = mConfig.getPort();
    const auto threadCount = mConfig.getThreadCount();
    const auto& dbName = mConfig.getDbName();
    auto dbPort = mConfig.getDbPort();
    
    Log::Debug(Print::composeMessage("Got command-line arguments:", "Port:", port, "Threads count:", threadCount));
    Log::Debug(Print::composeMessage("Got command-line db arguments:", "Port:", dbPort, "Db name:", dbName));

    mServer->configureDatabase(dbName, dbPort);
    Log::Debug("Database is initialized");

    mServer->start(port, threadCount);

    while (true) {
        Print::PrintLine("Enter command:");
        std::string command = Input::Read();
        handleCommand(command);
        if (command == "exit") {
            break;
        }
    }
}

} // namespace ResourceMonitorServer