#include "Controller.h"
#include "Input.h"
#include "Utils.h"

namespace ResourceMonitorClient {

Controller::Controller(Http::ClientPtr&& client)
    : mClient(std::move(client))
    , mIsValidState(true)
{
}

Controller::~Controller() {
    Log::Debug("Destroying controller");
}

void Controller::init(int argc, char* argv[]) {
    mIsValidState = mConfig.parseCommandLine(argc, argv);
}

void Controller::printHelpMessage() {
    Print::PrintLine("\nAvailable commands:");
    Print::PrintLine("  help                    - Display this help message");
    Print::PrintLine("  config                  - Enter configuration mode");
    Print::PrintLine("  request <resource> <count> <ip>");
    Print::PrintLine("    - Request resource monitoring data");
    Print::PrintLine("    - <resource>: basic_info, cpu, memory, disks, network");
    Print::PrintLine("    - <count>: number of measurements");
    Print::PrintLine("    - <ip>: target machine IP address");
    Print::PrintLine("  cancel <request_id>     - Cancel an ongoing request");
    Print::PrintLine("  exit                    - Exit the application\n");
}

void Controller::handleCommand(const std::string& command) {
    if (command == "exit") {
        Print::PrintLine("Stopping client...");
        mClient->close();
        Log::Info("Exiting application");
        return;
    }
    else if (command == "help") {
        printHelpMessage();
    }
    else if (command == "config") {
        mConfig.handleConfigCommand();
    }
    else if (command == "request") {
        std::string resource = Input::Read();
        std::string count = Input::Read();
        std::string ipAddress = Input::Read();

        static const std::set<std::string> validResources = {
            "basic_info",
            "cpu",
            "memory",
            "disks",
            "network"
        };

        bool isValidEndpoint = false;
        if (validResources.find(resource) != validResources.end()) {
            auto numberOpt = stringToInt(count, [](int value) { return value > 0; });
            if (!numberOpt) {
                isValidEndpoint = false;
            }
            else {
                isValidEndpoint = isValidIpAddress(ipAddress);
            }
        }
        std::string endpoint = resource + "/" + count + "/" + ipAddress;
        if (isValidEndpoint) {
            auto requestId = mClient->makeRequest(mConfig.getServerPort(), mConfig.getServerName(), resource, count, ipAddress);
            if (requestId) {
                Print::PrintLine(Print::composeMessage("Created request with id", *requestId, "Endpoint:", endpoint));
            }
            else {
                Print::PrintLine(Print::composeMessage("Failed to create request", endpoint));
            }
        }
        else {
            Print::PrintLine(Print::composeMessage("Incorrect endpoint provided", endpoint));
        }
    }
    else if (command == "cancel") {
        std::string id = Input::Read();
        mClient->cancelRequest(id);
    }
    else {
        Print::PrintLine("Unknown command. Type 'help' for available commands.");
        Log::Debug(Print::composeMessage("Unknown command entered:", command));
    }
}

void Controller::run() {
    if (!mIsValidState) {
        return;
    }

    const auto& printFilename = mConfig.getPrintFilename();
    if (printFilename.empty()) {
        Print::initConsolePrinter();
    }
    else {
        Print::initFilePrinter(printFilename);
    }

    const auto logFilename = mConfig.getLogFilename();
    const auto logLevel = mConfig.getLogLevel();
    if (logFilename.empty()) {
        Log::initConsoleLogger(logLevel);
    }
    else {
        Log::initFileLogger(logLevel, logFilename);
    }

    const auto& inputFilename = mConfig.getInputFilename();
    if (!inputFilename.empty()) {
        Input::initFileReader(inputFilename);
    }
    else {
        Input::initConsoleReader();
    }

    Log::Debug("Logger, printer and reader are initialized");

    auto port = mConfig.getServerPort();
    auto serverName = mConfig.getServerName();
    Log::Debug(Print::composeMessage("Get command line arguments. Port:", port, "Server name:", serverName));

    while (true) {
        Print::PrintLine("Enter command:");
        std::string command = Input::Read();
        handleCommand(command);
        if (command == "exit") {
            break;
        }
    }
}

} // namespace ResourceMonitorClient