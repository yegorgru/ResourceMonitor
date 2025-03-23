#pragma once

#include "Client.h"
#include "Config.h"

namespace ResourceMonitorClient {

class Controller
{
public:
    Controller(Client& client);
    ~Controller();
public:
    void init(int argc, char* argv[]);
    void run();
private:
    void printHelpMessage();
    void handleCommand(const std::string& command);
private:
    Client& mClient;
    Config mConfig;
    bool mIsValidState;
};

} // namespace ResourceMonitorClient