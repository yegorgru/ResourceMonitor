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
    mIsValidState = mArgumentParser.parseCommandLine(argc, argv);
}

void Controller::run() {
    if (!mIsValidState) {
        return;
    }

    const auto logFilename = mArgumentParser.getLogFilename();
    const auto logLevel = mArgumentParser.getLogLevel();
    if (logFilename == "") {
        LOG::initConsoleLogger(logLevel);
    }
    else {
        LOG::initFileLogger(logLevel, logFilename);
    }
    LOG::Debug("Logger is initialized");

    auto port = mArgumentParser.getServerPort();
    auto serverName = mArgumentParser.getServerName();
    LOG::Debug(LOG::composeMessage("Get command line arguments. Port:", port, "Server name:", serverName));

    std::string command;
    while (true) {
        LOG::SyncPrintLine("Enter command:", std::cout);
        std::cin >> command;
        if (command == "exit") {
            LOG::SyncPrintLine("Stopping client...", std::cout);
            mClient.close();
            LOG::Info("Exiting application");
            return;
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
                auto requestId = mClient.makeRequest(port, serverName, resource, count, ipAddress);
                if (requestId) {
                    LOG::SyncPrintLine(LOG::composeMessage("Created request with id", *requestId, "Endpoint:", endpoint), std::cout);
                }
                else {
                    LOG::SyncPrintLine(LOG::composeMessage("Failed to create request", endpoint), std::cout);
                }
            }
            else {
                LOG::SyncPrintLine(LOG::composeMessage("Incorrect endpoint provided", endpoint), std::cout);
            }
        }
        else if (command == "cancel") {
            std::string id;
            std::cin >> id;
            mClient.cancelRequest(id);
        }
        else {
            LOG::SyncPrintLine("Unknown command", std::cout);
            LOG::Debug(LOG::composeMessage("Unknown command entered:", command));
        }
    }
}

} // namespace ResourceMonitorClient