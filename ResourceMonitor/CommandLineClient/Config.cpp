#include "Config.h"
#include "Log.h"

#include <iostream>
#include <sstream>
#include <boost/program_options.hpp>

namespace ResourceMonitorClient {

namespace {
    const std::map<std::string, LogLevel> logLevelMap {
        {"throw", LogLevel::Throw},
        {"error", LogLevel::Error},
        {"warning", LogLevel::Warning},
        {"info", LogLevel::Info},
        {"debug", LogLevel::Debug},
        {"trace", LogLevel::Trace}
    };

    template<typename T>
    T getDefaultValue(const boost::program_options::options_description& desc, const std::string& option) {
        const auto& opt = desc.find(option, false);
        const boost::shared_ptr<const boost::program_options::value_semantic>& semantic = opt.semantic();
        boost::any defaultValue;
        semantic->apply_default(defaultValue);
        return boost::any_cast<T>(defaultValue);
    }
}

Config::Config()
    : mDescription("Allowed options")
{
    namespace po = boost::program_options;
    mDescription.add_options()
        ("help,h", "produce help message")
        ("server-port,p", po::value<int>()->default_value(3333), "server's port")
        ("server-name,n", po::value<std::string>()->default_value("localhost"), "server's name")
        ("log-level,l", po::value<std::string>()->default_value("info")->notifier(Config::validateLogLevel), "logging level: throw/error/warning/info/debug")
        ("log-file,L", po::value<std::string>()->default_value(""), "logging file");

    initializeConfigCommands();
}

void Config::initializeConfigCommands() {
    mConfigCommands["port"] = [this](const std::string& value) {
        std::istringstream iss(value);
        int port;
        iss >> port;
        
        if (iss.fail() || !iss.eof()) {
            LOG::SyncPrintLine("Invalid port value: " + value + ". Port must be a valid integer.", std::cout);
            return;
        }
        
        setServerPort(port);
        LOG::SyncPrintLine("Server port set to: " + value, std::cout);
    };
    mConfigHelp["port"] = "Set server port (e.g., port 3333)";

    mConfigCommands["server"] = [this](const std::string& value) {
        setServerName(value);
        LOG::SyncPrintLine("Server name set to: " + value, std::cout);
    };
    mConfigHelp["server"] = "Set server name (e.g., server localhost)";

    mConfigCommands["log-level"] = [this](const std::string& value) {
        try {
            setLogLevel(value);
            LOG::SyncPrintLine("Log level set to: " + value, std::cout);
        }
        catch (const std::exception&) {
            LOG::SyncPrintLine("Invalid log level: " + value + ". Valid values: throw/error/warning/info/debug/trace", std::cout);
        }
    };
    mConfigHelp["log-level"] = "Set log level (throw/error/warning/info/debug/trace)";

    mConfigCommands["log-file"] = [this](const std::string& value) {
        setLogFilename(value);
        LOG::SyncPrintLine("Log file set to: " + value, std::cout);
    };
    mConfigHelp["log-file"] = "Set log file path (e.g., log-file client.log)";

    mConfigCommands["show"] = [this](const std::string&) {
        showCurrentConfig();
    };
    mConfigHelp["show"] = "Show current configuration";

    mConfigCommands["help"] = [this](const std::string&) {
        showConfigHelp();
    };
    mConfigHelp["help"] = "Show this help message";

    mConfigCommands["no"] = [this](const std::string& value) {
        if (value == "port") {
            setServerPort(getDefaultValue<int>(mDescription, "server-port"));
            LOG::SyncPrintLine("Server port reset to default: " + std::to_string(getServerPort()), std::cout);
        }
        else if (value == "server") {
            setServerName(getDefaultValue<std::string>(mDescription, "server-name"));
            LOG::SyncPrintLine("Server name reset to default: " + getServerName(), std::cout);
        }
        else if (value == "log-level") {
            setLogLevel(getDefaultValue<std::string>(mDescription, "log-level"));
            LOG::SyncPrintLine("Log level reset to default: " + mVariablesMap["log-level"].as<std::string>(), std::cout);
        }
        else if (value == "log-file") {
            setLogFilename(getDefaultValue<std::string>(mDescription, "log-file"));
            LOG::SyncPrintLine("Log file reset to default (console output)", std::cout);
        }
        else {
            LOG::SyncPrintLine("Unknown config option: " + value, std::cout);
            LOG::SyncPrintLine("Available options: port, server, log-level, log-file", std::cout);
        }
    };
    mConfigHelp["no"] = "Reset config option to default (e.g., no port)";
}

void Config::handleConfigCommand() {
    LOG::SyncPrintLine("Entering config mode. Type 'help' for available commands or 'exit' to leave config mode.", std::cout);
    
    std::string line;
    while (std::getline(std::cin, line)) {
        if (line == "exit") {
            LOG::SyncPrintLine("Exiting config mode", std::cout);
            break;
        }

        std::istringstream iss(line);
        std::string command;
        iss >> command;

        if (command.empty()) {
            continue;
        }

        if (mConfigCommands.find(command) != mConfigCommands.end()) {
            std::string remaining;
            std::getline(iss >> std::ws, remaining);
            mConfigCommands[command](remaining);
        }
        else {
            LOG::SyncPrintLine("Unknown command: " + command, std::cout);
            LOG::SyncPrintLine("Type 'help' for available commands", std::cout);
        }
    }
}

void Config::showConfigHelp() const {
    LOG::SyncPrintLine("Available config commands:", std::cout);
    for (const auto& [command, help] : mConfigHelp) {
        LOG::SyncPrintLine("  " + command + ": " + help, std::cout);
    }
    LOG::SyncPrintLine("  exit: Leave config mode", std::cout);
}

bool Config::parseCommandLine(int argc, char* argv[]) {
    namespace po = boost::program_options;

    try {
        po::store(po::parse_command_line(argc, argv, mDescription), mVariablesMap);
        po::notify(mVariablesMap);
    }
    catch (const po::error& e) {
        LOG::SyncPrintLine("Failed to parse command line arguments: " + std::string(e.what()), std::cout);
        LOG::SyncPrintLine(LOG::composeMessage(mDescription), std::cout);
        return false;
    }
    if (mVariablesMap.count("help")) {
        LOG::SyncPrintLine(LOG::composeMessage(mDescription), std::cout);
        return false;
    }
    return true;
}

int Config::getServerPort() const {
    return mVariablesMap["server-port"].as<int>();
}

const std::string& Config::getServerName() const {
    return mVariablesMap["server-name"].as<std::string>();
}

const std::string& Config::getLogFilename() const {
    return mVariablesMap["log-file"].as<std::string>();
}

LogLevel Config::getLogLevel() const {
    const std::string logLevelStr = mVariablesMap["log-level"].as<std::string>();
    return logLevelMap.at(logLevelStr);
}

void Config::setServerPort(int port) {
    namespace po = boost::program_options;
    po::variable_value v(port, false);
    mVariablesMap.erase("server-port");
    mVariablesMap.insert(std::make_pair("server-port", v));
}

void Config::setServerName(const std::string& name) {
    namespace po = boost::program_options;
    po::variable_value v(name, false);
    mVariablesMap.erase("server-name");
    mVariablesMap.insert(std::make_pair("server-name", v));
}

void Config::setLogFilename(const std::string& filename) {
    namespace po = boost::program_options;
    po::variable_value v(filename, false);
    mVariablesMap.erase("log-file");
    mVariablesMap.insert(std::make_pair("log-file", v));
    reinitializeLogger();
}

void Config::setLogLevel(const std::string& level) {
    validateLogLevel(level);
    namespace po = boost::program_options;
    po::variable_value v(level, false);
    mVariablesMap.erase("log-level");
    mVariablesMap.insert(std::make_pair("log-level", v));
    reinitializeLogger();
}

void Config::validateLogLevel(const std::string& input) {
    namespace po = boost::program_options;
    if (logLevelMap.find(input) == logLevelMap.end()) {
        throw po::validation_error(po::validation_error::invalid_option_value, "log-level", input);
    }
}

void Config::reinitializeLogger() {
    const auto logFilename = getLogFilename();
    const auto logLevel = getLogLevel();
    LOG::destroyLogger();
    if (logFilename.empty()) {
        LOG::initConsoleLogger(logLevel);
    }
    else {
        LOG::initFileLogger(logLevel, logFilename);
    }
    LOG::Debug("Logger reinitialized");
}

void Config::showCurrentConfig() const {
    LOG::SyncPrintLine("\nCurrent configuration:", std::cout);
    LOG::SyncPrintLine("  Server port: " + std::to_string(getServerPort()), std::cout);
    LOG::SyncPrintLine("  Server name: " + getServerName(), std::cout);
    LOG::SyncPrintLine("  Log level: " + mVariablesMap["log-level"].as<std::string>(), std::cout);
    const auto& logFile = getLogFilename();
    LOG::SyncPrintLine("  Log file: " + (logFile.empty() ? "(console output)" : logFile), std::cout);
    LOG::SyncPrintLine("", std::cout);
}

} // namespace ResourceMonitorClient