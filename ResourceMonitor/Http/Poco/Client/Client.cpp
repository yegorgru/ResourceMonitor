#include "Client.h"
#include "Log.h"
#include "HttpCommon.h"
#include "Utils.h"
#include "Print.h"
#include "Poco/ThreadPoolManager.h"

#include <nlohmann/json.hpp>

namespace Http::Poco {

Client::Client()
{
    ThreadPoolManager::Init();
    ThreadPoolManager::Get().start(1, 1);
}

Client::~Client()
{
    Log::Debug("Destroying Poco client");
    close();
    ThreadPoolManager::Destroy();
}

Client::OptionalRequestId Client::makeRequest(Port serverPort, const std::string& serverName, const std::string& resource, const std::string& count, const std::string& ipAddress) {
    Log::Debug("Poco Client::makeRequest");
    
    auto callback = getCallback(resource);
    if (!callback) {
        return std::nullopt;
    }
    std::string endpoint = resource + "/" + count + "/" + ipAddress;
    auto session = std::make_shared<Session>(serverName, serverPort, *callback);
    Id id = session->getId();
    {
        std::lock_guard<std::mutex> lock(mSessionsMutex);
        mSessions[id] = session;
    }
    session->get(endpoint);
    return idToString(id);
}

void Client::cancelRequest(const std::string strId) {
    Log::Debug("Poco Client::cancelRequest");
    std::string message;
    auto idOpt = stringToId(strId);
    if (!idOpt) {
        Log::Info(Print::composeMessage("Failed to convert string to valid id. Id:", strId));
        return;
    }
    
    auto id = *idOpt;
    std::lock_guard<std::mutex> lock(mSessionsMutex);
    auto found = mSessions.find(id);
    
    if (found != mSessions.end()) {
        message = Print::composeMessage("Request canceled. Id:", strId);
        if (!found->second->isCompleted()) {
            found->second->cancel();
            Log::Info(message);
        }
        else {
            Log::Error(Print::composeMessage("Error: completed request still in storage. Id:", strId));
        }
        mSessions.erase(found);
        Log::Debug(Print::composeMessage("Canceled request removed from storage. Id:", id));
    }
    else {
        message = Print::composeMessage("No requests with such id in storage. Id:", strId);
        Log::Info(message);
    }
    
    Print::PrintLine(message);
}

void Client::close() {
    Log::Debug("Poco Client::close");
    
    std::lock_guard<std::mutex> lock(mSessionsMutex);
    for (auto& [id, session] : mSessions) {
        session->cancel();
    }
    mSessions.clear();
}

Client::OptionalCallback Client::getCallback(const std::string& resource) {
    using json = nlohmann::json;
    auto printCallback = Http::getPrintCallback(resource);
    if (!printCallback) {
        return std::nullopt;
    }
    else {
        return [this, printCallback](const Session::ResponseData& response, const Id& id) {
            if (response.mStatusCode == StatusCode::ClientClosedRequest) {
                Log::Debug("Callback for canceled request");
                return;
            }
            std::string message;
            if (response.mStatusCode == StatusCode::Ok) {
                auto parsedJson = json::parse(response.mBody);
                
                if (!parsedJson.empty() && !parsedJson.contains("error")) {
                    (*printCallback)(parsedJson);
                    
                    message = "Request processed successfully";
                    Log::Info(message);
                }
                else if (parsedJson.empty()) {
                    message = Print::composeMessage("Failed to parse json response", response.mBody);
                    Log::Error(message);
                }
                else {
                    message = Print::composeMessage("Error message from server", parsedJson["error"].get<std::string>());
                    Log::Error(message);
                }
            }
            else {
                message = Print::composeMessage("Failed to get info from server", static_cast<int>(response.mStatusCode), response.mBody);
                Log::Error(message);
            }
            
            Print::PrintLine(message);
            auto finishMessage = Print::composeMessage("Finished request:", id);
            Log::Info(finishMessage);
            Print::PrintLine(finishMessage);
            {
                std::lock_guard<std::mutex> lock(mSessionsMutex);
                mSessions.erase(id);
                Log::Debug(Print::composeMessage("Completed request removed from storage. Id:", id));
            }
        };
    }
}

} // namespace Http::Poco