#include "Controller.h"
#include "Log.h"

#include <iostream>

namespace ResourceMonitorServer {

Controller::Controller(Server& server)
    : mServer(server)
{
}

void Controller::init(int argc, char* argv[]) {
    mArgumentParser.parseCommandLine(argc, argv);
}

void Controller::run() {
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
}

void Controller::processCommand(const std::string& str) {

}

} // namespace ResourceMonitorServer