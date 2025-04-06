#include "Client.h"
#include "Log.h"
#include "HttpCommons.h"

#include <boost/uuid/uuid_io.hpp>

namespace Http::Beast {

Client::Client()
    : mWork(boost::asio::make_work_guard(mIoContext))
{
    mThreadIo = std::jthread([this]() { 
        mIoContext.run();
    });
}

Client::~Client()
{
    Log::Debug("Destroying client");
    if (mWork.owns_work()) {
        close();
    }
}

Client::OptionalRequestId Client::makeRequest(int serverPort, const std::string& serverName, const std::string& resource, const std::string& count, const std::string& ipAddress) {
    Log::Debug("Making request");
    auto callback = getCallback(resource);
    if (!callback) {
        return std::nullopt;
    }
    auto session = std::make_shared<Session>(mIoContext, serverName, serverPort, *callback);
    std::string endpoint = resource + "/" + count + "/" + ipAddress;
    session->get(endpoint);
    const auto& id = session->getId();
    {
        std::lock_guard<std::mutex> lg(mSessionsMutex);
        mSessions[id] = session;
    }
    return boost::uuids::to_string(id);
}

void Client::cancelRequest(const std::string strId) {
    Log::Debug("Canceling request");
    std::string message;
    try
    {
        Session::Id id = Commons::stringToId(strId);
        std::lock_guard<std::mutex> lg(mSessionsMutex);
        auto found = mSessions.find(id);
        if (found != mSessions.end()) {
            if (!found->second->isCompleted()) {
                found->second->cancel();
                message = Print::composeMessage("Request canceled. Id:", strId);
                Log::Info(message);
                mSessions.erase(found);
                Log::Debug(Print::composeMessage("Canceled request removed from storage. Id:", boost::uuids::to_string(id)));
            }
            else {
                message = Print::composeMessage("Error: completed request still in storage. Id:", strId);
                Log::Error(message);
            }
        }
        else {
            message = Print::composeMessage("No requests with such id in storage. Id:", strId);
            Log::Info(message);
        }
    }
    catch (const std::runtime_error&)
    {
        message = Print::composeMessage("Failed to convert string to valid id. Id:", strId);
        Log::Error(message);
    }
    Print::PrintLine(message);
}

void Client::close() {
    Log::Debug("Client close");
    mWork.reset();
    mThreadIo.join();
}

Client::OptionalCallback Client::getCallback(const std::string& resource) {
    using json = nlohmann::json;
    auto printCallback = Http::Commons::getPrintCallback(resource);
    if (!printCallback) {
        return std::nullopt;
    }
    else {
        return [this, printCallback](const Session::HttpResponse& response, const Session::Id& id) {
            auto status = response.result();
            if (static_cast<int>(response.result_int()) == Session::CANCELED_HTTP_STATUS) {
                Log::Debug("Callback for canceled request");
                return;
            }
            else {
                std::lock_guard<std::mutex> lg(mSessionsMutex);
                mSessions.erase(id);
                Log::Debug(Print::composeMessage("Completed request removed from storage. Id:", boost::uuids::to_string(id)));
            }
            std::string message;
            if (status == boost::beast::http::status::ok) {
                const auto& jsonStr = response.body();
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
                message = Print::composeMessage("Failed to get info from server", static_cast<int>(status), response.reason(), response.body());
                Log::Error(message);
            }
            Print::PrintLine(message);
            auto finishMessage = Print::composeMessage("Finished request:", boost::uuids::to_string(id));
            Log::Info(finishMessage);
            Print::PrintLine(finishMessage);
        };
    }
}

} // namespace Http::Beast