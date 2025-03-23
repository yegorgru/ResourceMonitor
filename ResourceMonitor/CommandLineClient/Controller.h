#pragma once

#include "Client.h"
#include "ArgumentParser.h"

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
private:
    Client& mClient;
    ArgumentParser mArgumentParser;
    bool mIsValidState;
};

} // namespace ResourceMonitorClient