#pragma once

#include "IClient.h"
#include "Config.h"

#include <memory>

namespace ResourceMonitorClient {

class Controller
{
public:
    Controller(Http::ClientPtr&& client);
    ~Controller();
public:
    void init(int argc, char* argv[]);
    void run();
private:
    void printHelpMessage();
    void handleCommand(const std::string& command);
private:
    Http::ClientPtr mClient;
    Config mConfig;
    bool mIsValidState;
};

} // namespace ResourceMonitorClient