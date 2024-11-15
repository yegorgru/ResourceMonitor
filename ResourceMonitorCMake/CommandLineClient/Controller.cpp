#include "Controller.h"

namespace ResourceMonitorClient {

Controller::Controller(Client& client)
    : mClient(client)
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

    auto port = mArgumentParser.getServerPort();
    auto serverName = mArgumentParser.getServerName();
    LOG::Debug(LOG::composeMessage("Get command line arguments. Port:", port, "Server name:", serverName));

    std::string command;
    while (true) {
        LOG::SyncPrintLine("Enter command:", std::cout);
        std::cin >> command;
        if (command == "exit") {
            LOG::SyncPrintLine("Stopping client...", std::cout);
            mClient.close();
            LOG::Info("Exiting application");
            return;
        }
        else if (command == "request") {
            mClient.makeRequest(port, serverName);
        }
        else if (command == "cancel") {
            mClient.cancelRequest();
        }
        else {
            LOG::SyncPrintLine("Unknown command", std::cout);
            LOG::Debug(LOG::composeMessage("Unknown command entered:", command));
        }
    }
}

} // namespace ResourceMonitorClient