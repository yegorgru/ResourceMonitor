#include "pch.h"
#include "Log.h"

#include <iostream>
#include <filesystem>
#include <syncstream>

void LOG::initConsoleLogger(LogLevel logLevel) {
	if (mLogger) {
		throw std::runtime_error("Logger is already initialized");
	}
	mLogger = std::make_unique<LoggerConsole>();
	mLogLevel = logLevel;
}

void LOG::initFileLogger(LogLevel logLevel, const std::string fileName) {
	if (mLogger) {
		throw std::runtime_error("Logger is already initialized");
	}
	mLogger = std::make_unique<LoggerFile>(fileName);
	mLogLevel = logLevel;
}

void LOG::Debug(const std::string& message, std::source_location location) {
	log(LogLevel::Debug, message, location);
}

void LOG::Info(const std::string& message, std::source_location location) {
	log(LogLevel::Info, message, location);
}

void LOG::Warning(const std::string& message, std::source_location location) {
	log(LogLevel::Warning, message, location);
}

void LOG::Error(const std::string& message, std::source_location location) {
	log(LogLevel::Error, message, location);
}

void LOG::Throw(const std::string& message, std::source_location location) {
	log(LogLevel::Throw, message, location);
}

void LOG::SyncPrintLine(const std::string& message, std::ostream& os) {
	std::osyncstream out{ os };
	out << message << std::endl;
}

void LOG::log(LogLevel messageLogLevel, const std::string& message, std::source_location location) {
	if (!mLogger) {
		throw std::runtime_error("Logger is not initialized");
	}
	if (messageLogLevel >= mLogLevel) {
		std::ostringstream oss;
		switch (messageLogLevel) {
		case LogLevel::Debug:
			oss << "[DEBUG]\t";
			break;
		case LogLevel::Info:
			oss << "[INFO]\t";
			break;
		case LogLevel::Warning:
			oss << "[WARN]\t";
			break;
		case LogLevel::Error:
			oss << "[ERROR]\t";
			break;
		case LogLevel::Throw:
			oss << "[THROW]\t";
			break;
		}
		std::filesystem::path filePath(location.file_name());
		oss << " " << filePath.filename().string() << ":" << location.line() << " | ";
		auto now = std::chrono::time_point_cast<std::chrono::seconds>(std::chrono::system_clock::now());
		oss << std::format("{:%Y-%m-%d %H:%M:%S}", now);
		oss << " | " << message;
		mLogger->printMessage(messageLogLevel, oss.str());
		if (messageLogLevel == LogLevel::Throw) {
			throw std::runtime_error(oss.str());
		}
	}
}

LOG::LoggerFile::LoggerFile(const std::string& fileName) 
	: mFile(fileName, std::ios::out | std::ios::trunc)
{
	if (!mFile.is_open()) {
		throw std::runtime_error("Failed to open file for logging");
	}
}

void LOG::LoggerFile::printMessage(LogLevel logLevel, const std::string& message) {
	SyncPrintLine(message, mFile);
}

void LOG::LoggerConsole::printMessage(LogLevel logLevel, const std::string& message) {
	if (logLevel <= LogLevel::Warning) {
		SyncPrintLine(message, std::cout);
	}
	else {
		SyncPrintLine(message, std::cerr);
	}
}