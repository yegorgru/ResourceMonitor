#include "ArgumentParser.h"
#include "Log.h"

#include <iostream>

namespace ResourceMonitorClient {

namespace {
    const std::map<std::string, LogLevel> logLevelMap {
        {"throw", LogLevel::Throw},
        {"error", LogLevel::Error},
        {"warning", LogLevel::Warning},
        {"info", LogLevel::Info},
        {"debug", LogLevel::Debug},
        {"trace", LogLevel::Trace},
    };
}

ArgumentParser::ArgumentParser()
    : mDescription("Allowed options")
{
    namespace po = boost::program_options;
    mDescription.add_options()
        ("help,h", "produce help message")
        ("server-port,p", po::value<int>()->default_value(3333), "server's port")
        ("server-name,n", po::value<std::string>()->default_value("localhost"), "server's name")
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

int ArgumentParser::getServerPort() const {
    return mVariablesMap["server-port"].as<int>();
}

const std::string& ArgumentParser::getServerName() const {
    return mVariablesMap["server-name"].as<std::string>();
}

const std::string& ArgumentParser::getLogFilename() const {
    return mVariablesMap["log-file"].as<std::string>();
}

LogLevel ArgumentParser::getLogLevel() const {
    const std::string logLevelStr = mVariablesMap["log-level"].as<std::string>();
    return logLevelMap.at(logLevelStr);
}

void ArgumentParser::validateLogLevel(const std::string& input) {
    namespace po = boost::program_options;
    if (logLevelMap.find(input) == logLevelMap.end()) {
        throw po::validation_error(po::validation_error::invalid_option_value, "log-level", input);
    }
}

} // namespace ResourceMonitorClient