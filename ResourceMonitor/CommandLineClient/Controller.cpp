#include "Controller.h"

namespace ResourceMonitorClient {

Controller::Controller(Client& client)
    : mClient(client)
    , mIsValidState(true)
{
}

Controller::~Controller() {
    LOG::Debug("Destroying controller");
}

void Controller::init(int argc, char* argv[]) {
    mIsValidState = mConfig.parseCommandLine(argc, argv);
}

void Controller::printHelpMessage() {
    PRINT::PrintLine("\nAvailable commands:");
    PRINT::PrintLine("  help                    - Display this help message");
    PRINT::PrintLine("  config                  - Enter configuration mode");
    PRINT::PrintLine("  request <resource> <count> <ip>");
    PRINT::PrintLine("    - Request resource monitoring data");
    PRINT::PrintLine("    - <resource>: basic_info, cpu, memory, disks, network");
    PRINT::PrintLine("    - <count>: number of measurements");
    PRINT::PrintLine("    - <ip>: target machine IP address");
    PRINT::PrintLine("  cancel <request_id>     - Cancel an ongoing request");
    PRINT::PrintLine("  exit                    - Exit the application\n");
}

void Controller::handleCommand(const std::string& command) {
    if (command == "exit") {
        PRINT::PrintLine("Stopping client...");
        mClient.close();
        LOG::Info("Exiting application");
        return;
    }
    else if (command == "help") {
        printHelpMessage();
    }
    else if (command == "config") {
        mConfig.handleConfigCommand();
    }
    else if (command == "request") {
        std::string resource;
        std::cin >> resource;
        std::string count;
        std::cin >> count;
        std::string ipAddress;
        std::cin >> ipAddress;

        static const std::set<std::string> validResources = {
            "basic_info",
            "cpu",
            "memory",
            "disks",
            "network"
        };

        bool isValidEndpoint = false;
        if (validResources.find(resource) != validResources.end()) {
            int number;
            if (boost::conversion::try_lexical_convert<int>(count, number) != false) {
                boost::system::error_code ec;
                boost::asio::ip::address::from_string(ipAddress, ec);
                isValidEndpoint = ec.value() == 0;
            }
        }
        std::string endpoint = resource + "/" + count + "/" + ipAddress;
        if (isValidEndpoint) {
            auto requestId = mClient.makeRequest(mConfig.getServerPort(), mConfig.getServerName(), resource, count, ipAddress);
            if (requestId) {
                PRINT::PrintLine(PRINT::composeMessage("Created request with id", *requestId, "Endpoint:", endpoint));
            }
            else {
                PRINT::PrintLine(PRINT::composeMessage("Failed to create request", endpoint));
            }
        }
        else {
            PRINT::PrintLine(PRINT::composeMessage("Incorrect endpoint provided", endpoint));
        }
    }
    else if (command == "cancel") {
        std::string id;
        std::cin >> id;
        mClient.cancelRequest(id);
    }
    else {
        PRINT::PrintLine("Unknown command. Type 'help' for available commands.");
        LOG::Debug(PRINT::composeMessage("Unknown command entered:", command));
    }
}

void Controller::run() {
    if (!mIsValidState) {
        return;
    }

    const auto& printFilename = mConfig.getPrintFilename();
    if (printFilename.empty()) {
        PRINT::initConsolePrinter();
    }
    else {
        PRINT::initFilePrinter(printFilename);
    }

    const auto logFilename = mConfig.getLogFilename();
    const auto logLevel = mConfig.getLogLevel();
    if (logFilename.empty()) {
        LOG::initConsoleLogger(logLevel);
    }
    else {
        LOG::initFileLogger(logLevel, logFilename);
    }
    LOG::Debug("Logger and printer are initialized");

    auto port = mConfig.getServerPort();
    auto serverName = mConfig.getServerName();
    LOG::Debug(PRINT::composeMessage("Get command line arguments. Port:", port, "Server name:", serverName));

    std::string command;
    while (true) {
        PRINT::PrintLine("Enter command:");
        std::cin >> command;
        handleCommand(command);
        if (command == "exit") {
            break;
        }
    }
}

} // namespace ResourceMonitorClient