#pragma once

#include "Log.h"

#include <boost/program_options.hpp>

namespace ResourceMonitorServer {

class ArgumentParser
{
public:
	ArgumentParser();
public:
	bool parseCommandLine(int argc, char* argv[]);
public:
	const std::string& getLogFilename() const;
	LogLevel getLogLevel() const;
	int getPort() const;
	int getThreadCount() const;
	int getDbPort() const;
	const std::string& getDbName() const;
private:
	static void validateLogLevel(const std::string& input);
private:
	using VariablesMap = boost::program_options::variables_map;
	using Description = boost::program_options::options_description;
private:
	VariablesMap mVariablesMap;
	Description mDescription;
};

} // namespace ResourceMonitorServer