#include "Client.h"
#include "Log.h"

#include <boost/uuid/uuid_io.hpp>
#include <nlohmann/json.hpp>

namespace ResourceMonitorClient {

Client::Client()
    : mWork(boost::asio::make_work_guard(mIoService))
{
    mThreadIo = std::jthread([this]() { 
        mIoService.run();
    });
    mThreadCleaner = std::jthread([this](std::stop_token stopToken) {
        using namespace std::chrono_literals;
        while (!stopToken.stop_requested()) {
            std::this_thread::sleep_for(3s);
            LOG::Trace("Start requests cleaning");
            std::lock_guard<std::mutex> lg(mRequestsMutex);
            for (auto it = mRequests.begin(); it != mRequests.end();) {
                if (!it->second.lock()) {
                    LOG::Debug(LOG::composeMessage("Erasing request entry from storage as it's completed or canceled. Id:", boost::uuids::to_string(it->first)));
                    it = mRequests.erase(it);
                }
                else {
                    ++it;
                }
            }
        }
        LOG::Debug("Exiting cleaner thread");
    });
}

Client::~Client()
{
    LOG::Debug("Destroying client");
    if (mWork.owns_work()) {
        close();
    }
}

std::string Client::makeRequest(int serverPort, const std::string& serverName) {
    using json = nlohmann::json;

    LOG::Debug("Making request");
    static auto clientCallback = [](const Http::MessageResponse& response, const Http::Request::Id& id) {
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
        auto finishMessage = LOG::composeMessage("Finished request:", boost::uuids::to_string(id));
        LOG::Info(finishMessage);
        LOG::SyncPrintLine(finishMessage, std::cout);
    };
    auto request = std::make_shared<Http::Request>(mIoService, serverName, serverPort, clientCallback);
    request->get("machine");
    const auto& id = request->getId();
    mRequests[id] = request;
    return boost::uuids::to_string(id);
}

void Client::cancelRequest(const std::string strId) {
    LOG::Debug("Canceling request");
    static boost::uuids::string_generator stringGen;
    std::string message;
    try
    {
        Http::Request::Id id = stringGen(strId);
        std::lock_guard<std::mutex> lg(mRequestsMutex);
        auto found = mRequests.find(id);
        if (found != mRequests.end()) {
            auto sharedPtr = found->second.lock();
            if (sharedPtr) {
                sharedPtr->cancel();
                message = LOG::composeMessage("Request canceled. Id:", strId);
                LOG::Info(message);
            }
            else {
                message = LOG::composeMessage("Request already completed. Id:", strId);
                LOG::Info(LOG::composeMessage(message));
            }
        }
        else {
            message = LOG::composeMessage("No requests with such id in storage. Id:", strId);
            LOG::Info(LOG::composeMessage(message));
        }
    }
    catch (const std::runtime_error&)
    {
        message = LOG::composeMessage("Failed to convert string to valid id. Id:", strId);
        LOG::Error(message);
    }
    LOG::SyncPrintLine(message, std::cout);
}

void Client::close() {
    LOG::Debug("Client close");
    mThreadCleaner.request_stop();
    mWork.reset();
    mThreadIo.join();
    mThreadCleaner.join();
}

} // namespace ResourceMonitorClient