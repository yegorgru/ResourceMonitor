#include "ArgumentParser.h"
#include "Log.h"

#include <iostream>

namespace ResourceMonitorClient {

ArgumentParser::ArgumentParser()
    : mDescription("Allowed options")
{
    namespace po = boost::program_options;
    mDescription.add_options()
        ("help,h", "produce help message")
        ("log-level,l", po::value<std::string>()->default_value("info")->notifier(ArgumentParser::validateLogLevel), "logging level: throw/error/warning/info/debug")
        ("log-file,L", po::value<std::string>()->default_value(""), "logging file");
}

bool ArgumentParser::parseCommandLine(int argc, char* argv[]) {
    namespace po = boost::program_options;

    try {
        po::store(po::parse_command_line(argc, argv, mDescription), mVariablesMap);
        po::notify(mVariablesMap);
    }
    catch (const po::error& e) {
        LOG::SyncPrintLine(LOG::composeMessage("Failed to parse command line arguments: ", e.what()), std::cout);
        LOG::SyncPrintLine(LOG::composeMessage(mDescription), std::cout);
        return false;
    }
    if (mVariablesMap.count("help")) {
        LOG::SyncPrintLine(LOG::composeMessage(mDescription), std::cout);
        return false;
    }
    return true;
}

const std::string& ArgumentParser::getLogFilename() const {
    return mVariablesMap["log-file"].as<std::string>();
}

LogLevel ArgumentParser::getLogLevel() const {
    const std::map<std::string, LogLevel> logLevelMap{
        {"throw", LogLevel::Throw},
        {"error", LogLevel::Error},
        {"warning", LogLevel::Warning},
        {"info", LogLevel::Info},
        {"debug", LogLevel::Debug},
    };
    const std::string logLevelStr = mVariablesMap["log-level"].as<std::string>();
    return logLevelMap.at(logLevelStr);
}

void ArgumentParser::validateLogLevel(const std::string& input) {
    namespace po = boost::program_options;
    const std::set<std::string> allowed_names = { "throw", "error", "warning", "info", "debug" };
    if (allowed_names.find(input) == allowed_names.end()) {
        throw po::validation_error(po::validation_error::invalid_option_value, "log-level", input);
    }
}

} // namespace ResourceMonitorClient