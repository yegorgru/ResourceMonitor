#pragma once

#include "IServer.h"
#include "Log.h"
#include <boost/program_options.hpp>
#include <functional>
#include <map>
#include <string>

namespace ResourceMonitorServer {

class Config
{
public:
    Config(Http::IServer& server);
public:
    bool parseCommandLine(int argc, char* argv[]);
    void handleConfigCommand();
public:
    const std::string& getLogFilename() const;
    LogLevel getLogLevel() const;
    int getPort() const;
    int getThreadCount() const;
    int getDbPort() const;
    const std::string& getDbName() const;
    const std::string& getIpAddress() const;
private:
    void setPort(int port);
    void setThreadCount(int count);
    void setDbPort(int port);
    void setDbName(const std::string& name);
    void setLogFilename(const std::string& filename);
    void setLogLevel(const std::string& level);
    void setIpAddress(const std::string& ip);
private:
    static void validateLogLevel(const std::string& input);
    static void validateIp(const std::string& input);
    void showConfigHelp() const;
    void showCurrentConfig() const;
    void initializeConfigCommands();
    void reinitializeLogger();
private:
    void restartServer();
private:
    using VariablesMap = boost::program_options::variables_map;
    using Description = boost::program_options::options_description;
    using ConfigCommand = std::function<void(const std::string&)>;
    using ConfigCommandsMap = std::map<std::string, ConfigCommand>;
    using ConfigHelpMap = std::map<std::string, std::string>;
private:
    Http::IServer& mServer;
    VariablesMap mVariablesMap;
    Description mDescription;
    ConfigCommandsMap mConfigCommands;
    ConfigHelpMap mConfigHelp;
};

} // namespace ResourceMonitorServer