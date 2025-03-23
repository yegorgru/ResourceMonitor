#pragma once

#include <sstream>
#include <string>
#include <utility>
#include <map>
#include <format>
#include <memory>
#include <stdexcept>
#include <chrono>
#include <source_location>
#include <fstream>

enum class LogLevel {
	Trace = 0,
	Debug,
	Info,
	Warning,
	Error,
	Throw
};

class LOG {
public:
	static void Trace(const std::string& message, std::source_location location = std::source_location::current());
	static void Debug(const std::string& message, std::source_location location = std::source_location::current());
	static void Info(const std::string& message, std::source_location location = std::source_location::current());
	static void Warning(const std::string& message, std::source_location location = std::source_location::current());
	static void Error(const std::string& message, std::source_location location = std::source_location::current());
	static void Throw(const std::string& message, std::source_location location = std::source_location::current());
	static void SyncPrintLine(const std::string& message, std::ostream& os);
public:
	template <typename... Args>
	static std::string composeMessage(Args&&... args);
public:
	static void initConsoleLogger(LogLevel logLevel);
	static void initFileLogger(LogLevel logLevel, const std::string fileName);
	static void destroyLogger();
private:
	static void log(LogLevel messageLogLevel, const std::string& message, std::source_location location);
private:
	class Logger {
	protected:
		Logger() = default;
	public:
		virtual void printMessage(LogLevel logLevel, const std::string& message) = 0;
	};

	class LoggerFile : public Logger
	{
	public:
		LoggerFile(const std::string& fileName);
	private:
		void printMessage(LogLevel logLevel, const std::string& message) override;
	private:
		std::ofstream mFile;
	};

	class LoggerConsole : public Logger
	{
	public:
		LoggerConsole() = default;
	private:
		void printMessage(LogLevel logLevel, const std::string& message) override;
	};
private:
	using LoggerPtr = std::unique_ptr<Logger>;
private:
	inline static LoggerPtr mLogger = nullptr;
	inline static LogLevel mLogLevel = LogLevel::Error;
};

template <typename... Args>
std::string LOG::composeMessage(Args&&... args) {
	std::ostringstream oss;
	((oss << args << " "), ...);
	std::string message = oss.str();
	if (message.length() > 0) {
		message.pop_back();
	}
	return message;
}