#pragma once

#include "Server.h"
#include "Config.h"

#include <boost/program_options.hpp>

namespace ResourceMonitorServer {

class Controller
{
public:
    Controller(Server& server);
public:
    void init(int argc, char* argv[]);
    void run();
private:
    void printHelpMessage();
    void handleCommand(const std::string& command);
private:
    Server& mServer;
    Config mConfig;
    bool mIsValidState;
};

} // namespace ResourceMonitorServer