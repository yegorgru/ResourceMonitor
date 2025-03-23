#pragma once

#include "Server.h"
#include "ArgumentParser.h"

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
private:
    Server& mServer;
    ArgumentParser mArgumentParser;
    bool mIsValidState;
};

} // namespace ResourceMonitorServer