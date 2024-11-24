#include "Client.h"
#include "Log.h"

#include <nlohmann/json.hpp>

namespace ResourceMonitorClient {

Client::Client()
    : mWork(boost::asio::make_work_guard(mIoService))
    , mThread([this]() { mIoService.run(); })
{
}

Client::~Client()
{
    LOG::Debug("Destroying client");
    if (mWork.owns_work()) {
        close();
    }
}

void Client::makeRequest(int serverPort, const std::string& serverName) {
    using json = nlohmann::json;

    LOG::Debug("Making request");
    static auto clientCallback = [](Http::MessageResponse& response) {
        auto statusCode = response.getStatusCode();
        std::string message;
        if (statusCode == Http::StatusCode::Ok) {
            const auto& jsonStr = response.getBody();
            auto parsedJson = json::parse(jsonStr);
            if (!parsedJson.empty() && !parsedJson.contains("error")) {
                //LOG::Info(LOG::composeMessage("Name:", machineState->mName));
                LOG::Info(LOG::composeMessage("CpuUsage:", parsedJson["cpu"]["usage %"]));
                LOG::Info(LOG::composeMessage("MemoryUsage:", parsedJson["memory"]["usage %"]));
                LOG::Info(LOG::composeMessage("TotalMemory:", parsedJson["memory"]["total GB"]));
                LOG::Info(LOG::composeMessage("MemoryUsed:", parsedJson["memory"]["used GB"]));
                LOG::Info(LOG::composeMessage("DiskUsage:", parsedJson["disk"]["usage %"]));
                LOG::Info(LOG::composeMessage("TotalDisk:", parsedJson["disk"]["total GB"]));
                LOG::Info(LOG::composeMessage("DiskUsed:", parsedJson["disk"]["used GB"]));

                message = "Request processed successfully";
                LOG::Info(message);
            }
            else if (parsedJson.empty()) {
                message = LOG::composeMessage("Failed to parse json response", jsonStr);
                LOG::Error(message);
            }
            else {
                message = LOG::composeMessage("Error message from server", parsedJson["error"]);
                LOG::Error(message);
            }
        }
        else {
            message = LOG::composeMessage("Failed to get info from server", static_cast<int>(statusCode), response.getStatusMessage(), response.getBody());
            LOG::Error(message);
        }
        LOG::SyncPrintLine(message, std::cout);
    };
    auto request = std::make_shared<Http::Request>(mIoService, serverName, serverPort, clientCallback);
    request->get("machine");
}

void Client::cancelRequest() {
    LOG::Debug("Canceling request");
}

void Client::close() {
    LOG::Debug("Client close");
    mWork.reset();
    mThread.join();
}

} // namespace ResourceMonitorClient