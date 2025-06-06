#include "Client.h"
#include "Log.h"
#include "HttpCommon.h"
#include "Utils.h"

namespace Http::Asio {

Client::Client()
    : mWork(boost::asio::make_work_guard(mIoService))
{
    mThreadIo = std::jthread([this]() { 
        mIoService.run();
    });
}

Client::~Client()
{
    Log::Debug("Destroying client");
    if (mWork.owns_work()) {
        close();
    }
}

Client::OptionalRequestId Client::makeRequest(Port serverPort, const std::string& serverName, const std::string& resource, const std::string& count, const std::string& ipAddress) {
    Log::Debug("Making request");
    auto callback = getCallback(resource);
    if (!callback) {
        return std::nullopt;
    }
    auto request = std::make_shared<Request>(mIoService, serverName, serverPort, *callback);
    std::string endpoint = resource + "/" + count + "/" + ipAddress;
    request->get(endpoint);
    const auto& id = request->getId();
    {
        std::lock_guard<std::mutex> lg(mRequestsMutex);
        mRequests[id] = request;
    }
    return idToString(id);
}

void Client::cancelRequest(const std::string strId) {
    Log::Debug("Canceling request");
    std::string message;
    auto idOpt = stringToId(strId);
    if (!idOpt) {
        Log::Error(Print::composeMessage("Failed to convert string to valid id. Id:", strId));
        return;
    }
    auto id = *idOpt;
    std::lock_guard<std::mutex> lg(mRequestsMutex);
    auto found = mRequests.find(id);
    if (found != mRequests.end()) {
        message = Print::composeMessage("Request canceled. Id:", strId);
        if (!found->second->isCompleted()) {
            found->second->cancel();
            Log::Info(message);
        }
        else {
            Log::Error(Print::composeMessage("Error: completed request still in storage. Id:", strId));
        }
        mRequests.erase(found);
        Log::Debug(Print::composeMessage("Canceled request removed from storage. Id:", id));
    }
    else {
        message = Print::composeMessage("No requests with such id in storage. Id:", strId);
        Log::Info(message);
    }
    Print::PrintLine(message);
}

void Client::close() {
    Log::Debug("Client close");
    std::lock_guard<std::mutex> lock(mRequestsMutex);
    for (auto& [id, request] : mRequests) {
        request->cancel();
    }
    mRequests.clear();
    mWork.reset();
    mThreadIo.join();
}

Client::OptionalCallback Client::getCallback(const std::string& resource) {
    using json = nlohmann::json;
    auto printCallback = Http::getPrintCallback(resource);
    if (!printCallback) {
        return std::nullopt;
    }
    else {
        return [this, printCallback](const MessageResponse& response, const Id& id) {
            auto statusCode = response.getStatusCode();
            if (statusCode == StatusCode::ClientClosedRequest) {
                Log::Debug("Callback for canceled request");
                return;
            }
            std::string message;
            if (statusCode == StatusCode::Ok) {
                const auto& jsonStr = response.getBody();
                auto parsedJson = json::parse(jsonStr);
                if (!parsedJson.empty() && !parsedJson.contains("error")) {

                    (*printCallback)(parsedJson);

                    message = "Request processed successfully";
                    Log::Info(message);
                }
                else if (parsedJson.empty()) {
                    message = Print::composeMessage("Failed to parse json response", jsonStr);
                    Log::Error(message);
                }
                else {
                    message = Print::composeMessage("Error message from server", parsedJson["error"]);
                    Log::Error(message);
                }
            }
            else {
                message = Print::composeMessage("Failed to get info from server", static_cast<int>(statusCode), response.getStatusMessage(), response.getBody());
                Log::Error(message);
            }
            Print::PrintLine(message);
            auto finishMessage = Print::composeMessage("Finished request:", id);
            Log::Info(finishMessage);
            Print::PrintLine(finishMessage);
            {
                std::lock_guard<std::mutex> lg(mRequestsMutex);
                mRequests.erase(id);
                Log::Debug(Print::composeMessage("Completed request removed from storage. Id:", id));
            }
        };
    }
}

} // namespace Http::Asio