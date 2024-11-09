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

    std::string command;
    while (true) {
        //std::cout << "> ";
        std::cin >> command;
        if (command == "exit") {
            std::cout << "Stopping client..." << std::endl;
            mClient.close();
            LOG::Info("Exiting application");
            return;
        }
        else if (command == "request") {
            mClient.makeRequest();
        }
        else if (command == "cancel") {
            mClient.cancelRequest();
        }
        else {
            std::cout << "Unknown command" << std::endl;
            LOG::Debug(LOG::makeLogMessage("Unknown command entered:", command));
        }
    }
}

} // namespace ResourceMonitorClient