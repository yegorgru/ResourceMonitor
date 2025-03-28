#pragma once

#include "Log.h"
#include <boost/program_options.hpp>
#include <functional>
#include <map>
#include <string>

namespace ResourceMonitorServer {

class Config
{
public:
    Config();
public:
    bool parseCommandLine(int argc, char* argv[]);
    void handleConfigCommand();
    bool isServerRestartNeeded() const;
    void resetServerRestartFlag();
public:
    const std::string& getLogFilename() const;
    LogLevel getLogLevel() const;
    int getPort() const;
    int getThreadCount() const;
    int getDbPort() const;
    const std::string& getDbName() const;
private:
    void setPort(int port);
    void setThreadCount(int count);
    void setDbPort(int port);
    void setDbName(const std::string& name);
    void setLogFilename(const std::string& filename);
    void setLogLevel(const std::string& level);
private:
    static void validateLogLevel(const std::string& input);
    void showConfigHelp() const;
    void showCurrentConfig() const;
    void initializeConfigCommands();
    void reinitializeLogger();
private:
    using VariablesMap = boost::program_options::variables_map;
    using Description = boost::program_options::options_description;
    using ConfigCommand = std::function<void(const std::string&)>;
private:
    VariablesMap mVariablesMap;
    Description mDescription;
    std::map<std::string, ConfigCommand> mConfigCommands;
    std::map<std::string, std::string> mConfigHelp;
    bool mServerRestartNeeded;
};

} // namespace ResourceMonitorServer