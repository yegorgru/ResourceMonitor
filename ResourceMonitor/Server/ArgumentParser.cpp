#include "ArgumentParser.h"

#include <iostream>

namespace ResourceMonitorServer {

ArgumentParser::ArgumentParser() 
	: mDescription("Allowed options")
{
    namespace po = boost::program_options;
    mDescription.add_options()
        ("help,h", "produce help message")
        ("port,p", po::value<int>()->default_value(3333), "server's port")
        ("log-level,l", po::value<std::string>()->default_value("info")->notifier(ArgumentParser::validateLogLevel), "logging level")
        ("log-file,L", po::value<std::string>()->default_value(""), "logging file")
        ("threads-count,t", po::value<int>()->default_value(2), "number of threads in server's pool");
}

bool ArgumentParser::parseCommandLine(int argc, char* argv[]) {
    namespace po = boost::program_options;

    try {
        po::store(po::parse_command_line(argc, argv, mDescription), mVariablesMap);
        po::notify(mVariablesMap);
    }
    catch (const po::error& e) {
        std::cout << "Failed to parse command line arguments: " << e.what() << std::endl;
        std::cout << mDescription << std::endl;
        return false;
    }
    if (mVariablesMap.count("help")) {
        std::cout << mDescription << std::endl;
        return false;
    }
}

const std::string& ArgumentParser::getLogFilename() const {
    return mVariablesMap["log-file"].as<std::string>();
}

LogLevel ArgumentParser::getLogLevel() const {
    const std::map<std::string, LogLevel> logLevelMap {
        {"throw", LogLevel::Throw},
        {"error", LogLevel::Error},
        {"warning", LogLevel::Warning},
        {"info", LogLevel::Info},
        {"debug", LogLevel::Debug},
    };
    const std::string logLevelStr = mVariablesMap["log-level"].as<std::string>();
    return logLevelMap.at(logLevelStr);
}

int ArgumentParser::getPort() const {
    return mVariablesMap["port"].as<int>();
}

int ArgumentParser::getThreadCount() const {
    return mVariablesMap["threads-count"].as<int>();
}

void ArgumentParser::validateLogLevel(const std::string& input) {
    namespace po = boost::program_options;
    const std::set<std::string> allowed_names = { "throw", "error", "warning", "info", "debug" };
    if (allowed_names.find(input) == allowed_names.end()) {
        throw po::validation_error(po::validation_error::invalid_option_value, "log-level", input);
    }
}

} // namespace ResourceMonitorServer