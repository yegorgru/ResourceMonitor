#pragma once

#include "Log.h"
#include "Input.h"
#include <boost/program_options.hpp>
#include <functional>
#include <map>
#include <string>

namespace ResourceMonitorClient {

class Config
{
public:
    Config();
public:
    bool parseCommandLine(int argc, char* argv[]);
    void handleConfigCommand();
public:
    int getServerPort() const;
    const std::string& getServerName() const;
    const std::string& getLogFilename() const;
    const std::string& getPrintFilename() const;
    const std::string& getInputFilename() const;
    LogLevel getLogLevel() const;
private:
    void setServerPort(int port);
    void setServerName(const std::string& name);
    void setLogFilename(const std::string& filename);
    void setPrintFilename(const std::string& filename);
    void setInputFilename(const std::string& filename);
    void setLogLevel(const std::string& level);
private:
    static void validateLogLevel(const std::string& input);
    void showConfigHelp() const;
    void showCurrentConfig() const;
    void initializeConfigCommands();
    void reinitializeLogger();
    void reinitializePrinter();
    void reinitializeReader();
private:
    using VariablesMap = boost::program_options::variables_map;
    using Description = boost::program_options::options_description;
    using ConfigCommand = std::function<void(const std::string&)>;
    using ConfigCommandsMap = std::map<std::string, ConfigCommand>;
    using ConfigHelpMap = std::map<std::string, std::string>;
private:
    VariablesMap mVariablesMap;
    Description mDescription;
    ConfigCommandsMap mConfigCommands;
    ConfigHelpMap mConfigHelp;
};

} // namespace ResourceMonitorClient