#pragma once

#include "IServer.h"
#include "Config.h"

namespace ResourceMonitorServer {

class Controller
{
public:
    Controller(Http::ServerPtr&& server);
public:
    void init(int argc, char* argv[]);
    void run();
private:
    void printHelpMessage();
    void handleCommand(const std::string& command);
private:
    Http::ServerPtr mServer;
    Config mConfig;
    bool mIsValidState;
};

} // namespace ResourceMonitorServer