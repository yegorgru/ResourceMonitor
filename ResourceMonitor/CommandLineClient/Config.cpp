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
        ("log-file,L", po::value<std::string>()->default_value(""), "logging file")
        ("print-file,P", po::value<std::string>()->default_value(""), "output print file")
        ("input-file,i", po::value<std::string>()->default_value(""), "input commands from file");

    initializeConfigCommands();
}

void Config::initializeConfigCommands() {
    mConfigCommands["port"] = [this](const std::string& value) {
        std::istringstream iss(value);
        int port;
        iss >> port;
        
        if (iss.fail() || !iss.eof()) {
            Print::PrintLine("Invalid port value: " + value + ". Port must be a valid integer.");
            return;
        }
        
        setServerPort(port);
        Print::PrintLine("Server port set to: " + value);
    };
    mConfigHelp["port"] = "Set server port (e.g., port 3333)";

    mConfigCommands["server"] = [this](const std::string& value) {
        setServerName(value);
        Print::PrintLine("Server name set to: " + value);
    };
    mConfigHelp["server"] = "Set server name (e.g., server localhost)";

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
    mConfigHelp["log-file"] = "Set log file path (e.g., log-file client.log)";

    mConfigCommands["print-file"] = [this](const std::string& value) {
        setPrintFilename(value);
        Print::PrintLine("Print file set to: " + value);
    };
    mConfigHelp["print-file"] = "Set output print file path (e.g., print-file output.txt)";

    mConfigCommands["input-file"] = [this](const std::string& value) {
        setInputFilename(value);
        Print::PrintLine("Input file set to: " + value);
    };
    mConfigHelp["input-file"] = "Set input file path (e.g., input-file commands.txt)";

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
            Print::PrintLine("Server port reset to default: " + std::to_string(getServerPort()));
        }
        else if (value == "server") {
            setServerName(getDefaultValue<std::string>(mDescription, "server-name"));
            Print::PrintLine("Server name reset to default: " + getServerName());
        }
        else if (value == "log-level") {
            setLogLevel(getDefaultValue<std::string>(mDescription, "log-level"));
            Print::PrintLine("Log level reset to default: " + mVariablesMap["log-level"].as<std::string>());
        }
        else if (value == "log-file") {
            setLogFilename(getDefaultValue<std::string>(mDescription, "log-file"));
            Print::PrintLine("Log file reset to default (console output)");
        }
        else if (value == "print-file") {
            setPrintFilename(getDefaultValue<std::string>(mDescription, "print-file"));
            Print::PrintLine("Print file reset to default (console output)");
        }
        else if (value == "input-file") {
            setInputFilename(getDefaultValue<std::string>(mDescription, "input-file"));
            Print::PrintLine("Input file reset to default (console input)");
        }
        else {
            Print::PrintLine("Unknown config option: " + value);
            Print::PrintLine("Available options: port, server, log-level, log-file, print-file");
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
    Print::PrintLine("  exit: Leave config mode");
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

int Config::getServerPort() const {
    return mVariablesMap["server-port"].as<int>();
}

const std::string& Config::getServerName() const {
    return mVariablesMap["server-name"].as<std::string>();
}

const std::string& Config::getLogFilename() const {
    return mVariablesMap["log-file"].as<std::string>();
}

const std::string& Config::getPrintFilename() const {
    return mVariablesMap["print-file"].as<std::string>();
}

const std::string& Config::getInputFilename() const {
    return mVariablesMap["input-file"].as<std::string>();
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

void Config::setPrintFilename(const std::string& filename) {
    namespace po = boost::program_options;
    po::variable_value v(filename, false);
    mVariablesMap.erase("print-file");
    mVariablesMap.insert(std::make_pair("print-file", v));
    reinitializePrinter();
}

void Config::setInputFilename(const std::string& filename) {
    namespace po = boost::program_options;
    po::variable_value v(filename, false);
    mVariablesMap.erase("input-file");
    mVariablesMap.insert(std::make_pair("input-file", v));
    reinitializeReader();
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

void Config::reinitializePrinter() {
    const auto& printFile = getPrintFilename();
    Print::destroyPrinter();
    if (printFile.empty()) {
        Print::initConsolePrinter();
    }
    else {
        Print::initFilePrinter(printFile);
    }
    Log::Debug("Printer reinitialized");
}

void Config::reinitializeReader() {
    const auto& inputFile = getInputFilename();
    Input::destroyReader();
    if (inputFile.empty()) {
        Input::initConsoleReader();
    }
    else {
        Input::initFileReader(inputFile);
    }
    Log::Debug("Reader reinitialized");
}

void Config::showCurrentConfig() const {
    Print::PrintLine("\nCurrent configuration:");
    Print::PrintLine("  Server port: " + std::to_string(getServerPort()));
    Print::PrintLine("  Server name: " + getServerName());
    Print::PrintLine("  Log level: " + mVariablesMap["log-level"].as<std::string>());
    const auto& logFile = getLogFilename();
    Print::PrintLine("  Log file: " + (logFile.empty() ? "(console output)" : logFile));
    const auto& printFile = getPrintFilename();
    Print::PrintLine("  Print file: " + (printFile.empty() ? "(console output)" : printFile));
    const auto& inputFile = getInputFilename();
    Print::PrintLine("  Input file: " + (inputFile.empty() ? "(console input)" : inputFile));
    Print::PrintLine("");
}

} // namespace ResourceMonitorClient