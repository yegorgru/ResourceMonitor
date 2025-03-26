#include "Print.h"

#include <iostream>
#include <syncstream>

void PRINT::initConsolePrinter() {
	if (mPrinter) {
        PrintLine("Printer will be reinialized. Previous printer will be destroyed.");
        destroyPrinter();
	}
    mPrinter = std::make_unique<ConsolePrinter>();
}

void PRINT::initFilePrinter(const std::string& fileName) {
	if (mPrinter) {
        PrintLine("Printer will be reinialized. Previous printer will be destroyed.");
        destroyPrinter();
	}
    mPrinter = std::make_unique<FilePrinter>(fileName);
}

void PRINT::destroyPrinter() {
    if (mPrinter) {
        mPrinter.reset();
    }
}

void PRINT::Print(const std::string& message) {
	if (!mPrinter) {
        initConsolePrinter();
        PrintLine("Printer is not initialized. Default console printer will be used.");
	}
    mPrinter->printMessage(message);
}

void PRINT::PrintLine(const std::string& message) {
    Print(message + "\n");
}

void PRINT::Print(const std::string& message, std::ostream& stream) {
	std::osyncstream out{ stream };
	out << message;
    out.flush();
}

void PRINT::PrintLine(const std::string& message, std::ostream& stream) {
    Print(message + "\n", stream);
}

PRINT::FilePrinter::FilePrinter(const std::string& fileName)
    : mFile(fileName, std::ios::out | std::ios::trunc)
{
	if (!mFile.is_open()) {
		throw std::runtime_error("Failed to open file for printing: " + fileName);
	}
}

PRINT::FilePrinter::~FilePrinter() {
    if (mFile.is_open()) {
        mFile.close();
    }
}

void PRINT::FilePrinter::printMessage(const std::string& message) {
    PRINT::Print(message, mFile);
}

void PRINT::ConsolePrinter::printMessage(const std::string& message) {
    PRINT::Print(message, std::cout);
}