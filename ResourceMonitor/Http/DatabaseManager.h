#pragma once

#include "Singleton.h"
#include "IoService.h"
#include "Log.h"
#include "Utils.h"

#include <string>
#include <memory>

namespace Http::Commons {

template <typename IoServiceType, typename RequestType, typename CallbackType = typename RequestType::Callback>
class DatabaseManager : public Singleton<DatabaseManager<IoServiceType, RequestType, CallbackType>>
{
    friend class Singleton<DatabaseManager<IoServiceType, RequestType, CallbackType>>;
public:
    void get(const std::string& endpoint, CallbackType callback) const;
    void put(const std::string& endpoint, const std::string& info, CallbackType callback);
private:
    DatabaseManager(const std::string& name, int port);
private:
    using RequestPtr = std::shared_ptr<RequestType>;
private:
    void createTimerForRequest(RequestPtr request) const;
private:
    std::string mName;
    int mPort;
};

template <typename IoServiceType, typename RequestType, typename CallbackType>
void DatabaseManager<IoServiceType, RequestType, CallbackType>::get(const std::string& endpoint, CallbackType callback) const {
    auto request = std::make_shared<RequestType>(IoService<IoServiceType>::Get().getIoService(), mName, mPort, callback);
    request->get(endpoint);
    createTimerForRequest(request);
}

template <typename IoServiceType, typename RequestType, typename CallbackType>
void DatabaseManager<IoServiceType, RequestType, CallbackType>::put(const std::string& endpoint, const std::string& info, CallbackType callback) {
    auto request = std::make_shared<RequestType>(IoService<IoServiceType>::Get().getIoService(), mName, mPort, callback);
    Log::Debug(Print::composeMessage("Sending info to database:", info));
    request->addHeader("Content-Type", "application/json");
    request->addHeader("Content-Length", std::to_string(info.length()));
    request->put(endpoint, info);
    createTimerForRequest(request);
}

template <typename IoServiceType, typename RequestType, typename CallbackType>
DatabaseManager<IoServiceType, RequestType, CallbackType>::DatabaseManager(const std::string& name, int port)
{
    if(isValidPort(port) && !name.empty()) {
        mName = name;
        mPort = port;
        Log::Debug(Print::composeMessage("DatabaseManager created. Name:", name, "Port:", port));
    }
    else {
        Log::Throw(Print::composeMessage("Invalid parameters for DatabaseManager. Name:", name, ". Port:", port));
    }
}

template <typename IoServiceType, typename RequestType, typename CallbackType>
void DatabaseManager<IoServiceType, RequestType, CallbackType>::createTimerForRequest(RequestPtr request) const {
    auto& ioService = IoService<IoServiceType>::Get().getIoService();
    auto timer = std::make_shared<boost::asio::steady_timer>(ioService);
    timer->expires_after(std::chrono::seconds(30));
    timer->async_wait([request, timer](const boost::system::error_code& ec) {
        if (!request->isCompleted()) {
            Log::Error("Failed to receive response from database for 30 seconds. Canceling request");
            request->cancel();
        }
        else {
            Log::Debug("Request to database finished less than in 30 seconds");
        }
    });
}

} // namespace Http::Commons