#pragma once

#include "Log.h"

#include <boost/program_options.hpp>

namespace ResourceMonitorClient {

class ArgumentParser
{
public:
	ArgumentParser();
public:
	bool parseCommandLine(int argc, char* argv[]);
public:
	int getServerPort() const;
	const std::string& getServerName() const;
	const std::string& getLogFilename() const;
	LogLevel getLogLevel() const;
private:
	static void validateLogLevel(const std::string& input);
private:
	using VariablesMap = boost::program_options::variables_map;
	using Description = boost::program_options::options_description;
private:
	VariablesMap mVariablesMap;
	Description mDescription;
};

} // namespace ResourceMonitorClient