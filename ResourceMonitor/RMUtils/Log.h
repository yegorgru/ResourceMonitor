#pragma once

#include <sstream>
#include <string>
#include <utility>
#include <map>
#include <format>
#include <memory>
#include <stdexcept>
#include <chrono>

enum class LogLevel {
	Debug = 0,
	Info,
	Warning,
	Error,
	Throw
};

class LOG {
public:
	template <typename... Args>
	static void Debug(Args&&... args);
	template <typename... Args>
	static void Info(Args&&... args);
	template <typename... Args>
	static void Warning(Args&&... args);
	template <typename... Args>
	static void Error(Args&&... args);
	template <typename... Args>
	static void Throw(Args&&... args);
public:
	static void initConsoleLogger(LogLevel logLevel);
	static void initFileLogger(LogLevel logLevel, const std::string fileName);
private:
	template <typename... Args>
	static void log(LogLevel messageLogLevel, Args&&... args);
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
		std::string mFileName;
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
void LOG::Debug(Args&&... args) {
	log(LogLevel::Debug, std::forward<Args>(args)...);
}

template <typename... Args>
void LOG::Info(Args&&... args) {
	log(LogLevel::Info, std::forward<Args>(args)...);
}

template <typename... Args>
void LOG::Warning(Args&&... args) {
	log(LogLevel::Warning, std::forward<Args>(args)...);
}

template <typename... Args>
void LOG::Error(Args&&... args) {
	log(LogLevel::Error, std::forward<Args>(args)...);
}

template <typename... Args>
void LOG::Throw(Args&&... args) {
	log(LogLevel::Throw, std::forward<Args>(args)...);
}

template <typename... Args>
void LOG::log(LogLevel messageLogLevel, Args&&... args) {
	if (!mLogger) {
		throw std::runtime_error("Logger is not initialized");
	}
	if (messageLogLevel >= mLogLevel) {
		std::ostringstream oss;
		switch (messageLogLevel) {
		case LogLevel::Debug:
			oss << "DEBUG\t";
			break;
		case LogLevel::Info:
			oss << "INFO\t";
			break;
		case LogLevel::Warning:
			oss << "WARNING\t";
			break;
		case LogLevel::Error:
			oss << "ERROR\t";
			break;
		case LogLevel::Throw:
			oss << "THROW\t";
			break;
		}
		oss << " | ";
		auto now = std::chrono::system_clock::now();
		oss << std::format("{0:%F_%T}", now);
		oss << " |";
		((oss << ' ' << args), ...);
		mLogger->printMessage(messageLogLevel, oss.str());
		if (messageLogLevel == LogLevel::Throw) {
			throw std::runtime_error(oss.str());
		}
	}
}