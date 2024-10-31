#include "pch.h"
#include "Log.h"

#include <iostream>

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

LOG::LoggerFile::LoggerFile(const std::string& fileName) 
	: mFileName(fileName)
{
}

void LOG::LoggerFile::printMessage(LogLevel logLevel, const std::string& message) {

}

void LOG::LoggerConsole::printMessage(LogLevel logLevel, const std::string& message) {
	if (logLevel <= LogLevel::Warning) {
		std::cout << message << std::endl;
	}
	else {
		std::cerr << message << std::endl;
	}
}