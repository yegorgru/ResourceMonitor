#include "Config.h"
#include "Log.h"
#include "DatabaseManager.h"
#include "Input.h"

#include <iostream>
#include <sstream>
#include <boost/program_options.hpp>

namespace ResourceMonitorServer {

namespace {
    const std::map<std::string, LogLevel> logLevelMap{
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
    , mServerRestartNeeded(false)
{
    namespace po = boost::program_options;
    mDescription.add_options()
        ("help,h", "produce help message")
        ("db-port,d", po::value<int>()->default_value(10000), "db's port")
        ("db-name,n", po::value<std::string>()->default_value("localhost"), "db's name")
        ("port,p", po::value<int>()->default_value(3333), "server's port")
        ("log-level,l", po::value<std::string>()->default_value("info")->notifier(Config::validateLogLevel), "logging level: throw/error/warning/info/debug")
        ("log-file,L", po::value<std::string>()->default_value(""), "logging file")
        ("threads-count,t", po::value<int>()->default_value(2), "number of threads in server's pool");

    initializeConfigCommands();
}

void Config::initializeConfigCommands() {
    auto validateIntInput = [](const std::string& value, const auto& condition) -> std::optional<int> {
        std::istringstream iss(value);
        int result;
        iss >> result;
        if (iss.fail() || !iss.eof() || !condition(result)) {
            return std::nullopt;
        }
        return result;
    };

    mConfigCommands["port"] = [this, validateIntInput](const std::string& value) {
        auto portOpt = validateIntInput(value, [](int port) { 
            return port >= 0 && port <= 65535; 
        });
        if (!portOpt) {
            Print::PrintLine("Invalid port value: " + value + ". Port must be a valid integer between 0 and 65535.");
            return;
        }
        setPort(*portOpt);
        Print::PrintLine("Server port set to: " + value);
    };
    mConfigHelp["port"] = "Set server port (e.g., port 3333). Change will be applied after leaving config mode.";

    mConfigCommands["threads"] = [this, validateIntInput](const std::string& value) {
        auto threadCountOpt = validateIntInput(value, [](int count) { 
            return count >= 2; 
        });
        if (!threadCountOpt) {
            Print::PrintLine("Invalid thread count: " + value + ". Must be an integer bigger than 2.");
            return;
        }       
        setThreadCount(*threadCountOpt);
        Print::PrintLine("Thread count set to: " + value);
    };
    mConfigHelp["threads"] = "Set thread pool size (e.g., threads 4). Change will be applied after leaving config mode.";

    mConfigCommands["db-port"] = [this, validateIntInput](const std::string& value) {
        auto dbPortOpt = validateIntInput(value, [](int port) { 
            return port >= 0 && port <= 65535; 
        });
        if (!dbPortOpt) {
            Print::PrintLine("Invalid db port value: " + value + ". Port must be a valid integer between 0 and 65535.");
            return;
        }        
        setDbPort(*dbPortOpt);
        Print::PrintLine("Database port set to: " + value);
    };
    mConfigHelp["db-port"] = "Set database port (e.g., db-port 10000)";

    mConfigCommands["db-name"] = [this](const std::string& value) {
        setDbName(value);
        Print::PrintLine("Database name set to: " + value);
    };
    mConfigHelp["db-name"] = "Set database name (e.g., db-name localhost)";

    mConfigCommands["log-level"] = [this](const std::string& value) {
        try {
            setLogLevel(value);
            Print::PrintLine("Log level set to: " + value);
        }
        catch (const std::exception&) {
            Print::PrintLine("Invalid log level: " + value + ". Valid values: throw/error/warning/info/debug/trace");
        }
    };
    mConfigHelp["log-level"] = "Set log level (throw/error/warning/info/debug/trace)";

    mConfigCommands["log-file"] = [this](const std::string& value) {
        setLogFilename(value);
        Print::PrintLine("Log file set to: " + value);
    };
    mConfigHelp["log-file"] = "Set log file path (e.g., log-file server.log)";

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
            setPort(getDefaultValue<int>(mDescription, "port"));
            Print::PrintLine("Server port reset to default: " + std::to_string(getPort()));
        }
        else if (value == "threads") {
            setThreadCount(getDefaultValue<int>(mDescription, "threads-count"));
            Print::PrintLine("Thread count reset to default: " + std::to_string(getThreadCount()));
        }
        else if (value == "db-port") {
            setDbPort(getDefaultValue<int>(mDescription, "db-port"));
            Print::PrintLine("Database port reset to default: " + std::to_string(getDbPort()));
        }
        else if (value == "db-name") {
            setDbName(getDefaultValue<std::string>(mDescription, "db-name"));
            Print::PrintLine("Database name reset to default: " + getDbName());
        }
        else if (value == "log-level") {
            setLogLevel(getDefaultValue<std::string>(mDescription, "log-level"));
            Print::PrintLine("Log level reset to default: " + mVariablesMap["log-level"].as<std::string>());
        }
        else if (value == "log-file") {
            setLogFilename(getDefaultValue<std::string>(mDescription, "log-file"));
            Print::PrintLine("Log file reset to default (console output)");
        }
        else {
            Print::PrintLine("Unknown config option: " + value);
            Print::PrintLine("Available options: port, threads, db-port, db-name, log-level, log-file");
        }
    };
    mConfigHelp["no"] = "Reset config option to default (e.g., no port)";
}

void Config::handleConfigCommand() {
    Print::PrintLine("Entering config mode. Type 'help' for available commands or 'exit' to leave config mode.");
    
    std::string line;
    while (true) {
        line = Input::ReadLine();
        
        if (line == "exit") {
            Print::PrintLine("Exiting config mode");
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
            Print::PrintLine("Unknown command: " + command);
            Print::PrintLine("Type 'help' for available commands");
        }
    }
}

void Config::showConfigHelp() const {
    Print::PrintLine("Available config commands:");
    for (const auto& [command, help] : mConfigHelp) {
        Print::PrintLine("  " + command + ": " + help);
    }
    Print::PrintLine("  exit: Leave config mode. Server will restart if needed to apply changes");
}

void Config::showCurrentConfig() const {
    Print::PrintLine("\nCurrent configuration:");
    Print::PrintLine("  Server port: " + std::to_string(getPort()));
    Print::PrintLine("  Thread count: " + std::to_string(getThreadCount()));
    Print::PrintLine("  Database port: " + std::to_string(getDbPort()));
    Print::PrintLine("  Database name: " + getDbName());
    Print::PrintLine("  Log level: " + mVariablesMap["log-level"].as<std::string>());
    const auto& logFile = getLogFilename();
    Print::PrintLine("  Log file: " + (logFile.empty() ? "(console output)" : logFile));
    Print::PrintLine("");
}

bool Config::parseCommandLine(int argc, char* argv[]) {
    namespace po = boost::program_options;

    try {
        po::store(po::parse_command_line(argc, argv, mDescription), mVariablesMap);
        po::notify(mVariablesMap);
    }
    catch (const po::error& e) {
        Print::PrintLine("Failed to parse command line arguments: " + std::string(e.what()));
        Print::PrintLine(Print::composeMessage(mDescription));
        return false;
    }
    if (mVariablesMap.count("help")) {
        Print::PrintLine(Print::composeMessage(mDescription));
        return false;
    }
    return true;
}

const std::string& Config::getLogFilename() const {
    return mVariablesMap["log-file"].as<std::string>();
}

LogLevel Config::getLogLevel() const {
    const std::string logLevelStr = mVariablesMap["log-level"].as<std::string>();
    return logLevelMap.at(logLevelStr);
}

int Config::getPort() const {
    return mVariablesMap["port"].as<int>();
}

int Config::getThreadCount() const {
    return mVariablesMap["threads-count"].as<int>();
}

int Config::getDbPort() const {
    return mVariablesMap["db-port"].as<int>();
}

const std::string& Config::getDbName() const {
    return mVariablesMap["db-name"].as<std::string>();
}

void Config::setPort(int port) {
    namespace po = boost::program_options;
    po::variable_value v(port, false);
    mVariablesMap.erase("port");
    mVariablesMap.insert(std::make_pair("port", v));
    mServerRestartNeeded = true;
}

void Config::setThreadCount(int count) {
    namespace po = boost::program_options;
    po::variable_value v(count, false);
    mVariablesMap.erase("threads-count");
    mVariablesMap.insert(std::make_pair("threads-count", v));
    mServerRestartNeeded = true;
}

void Config::setDbPort(int port) {
    namespace po = boost::program_options;
    po::variable_value v(port, false);
    mVariablesMap.erase("db-port");
    mVariablesMap.insert(std::make_pair("db-port", v));

    DatabaseManager::Destroy();
    DatabaseManager::Init(getDbName(), port);
    Log::Debug("Database reinitialized with new port");
}

void Config::setDbName(const std::string& name) {
    namespace po = boost::program_options;
    po::variable_value v(name, false);
    mVariablesMap.erase("db-name");
    mVariablesMap.insert(std::make_pair("db-name", v));

    DatabaseManager::Destroy();
    DatabaseManager::Init(name, getDbPort());
    Log::Debug("Database reinitialized with new name");
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
    Log::destroyLogger();
    if (logFilename.empty()) {
        Log::initConsoleLogger(logLevel);
    }
    else {
        Log::initFileLogger(logLevel, logFilename);
    }
    Log::Debug("Logger reinitialized");
}

bool Config::isServerRestartNeeded() const { 
    return mServerRestartNeeded;
}

void Config::resetServerRestartFlag() { 
    mServerRestartNeeded = false; 
}

} // namespace ResourceMonitorServer