#include "DatabaseManager.h"
#include "IoService.h"
#include "Log.h"
#include "Utils.h"

namespace Http::Asio {

namespace {
    void createTimerForRequest(RequestPtr request) {
        auto& ioService = IoService::Get().getIoService();
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
}

void DatabaseManager::get(const std::string& endpoint, Request::Callback callback) const {
    auto request = std::make_shared<Request>(IoService::Get().getIoService(), mName, mPort, callback);
    request->get(endpoint);
    createTimerForRequest(request);
}

void DatabaseManager::put(const std::string& endpoint, const std::string& info, Request::Callback callback) {
    auto request = std::make_shared<Request>(IoService::Get().getIoService(), mName, mPort, callback);
    Log::Debug(Print::composeMessage("Sending info to database:", info));
    request->addHeader("Content-Type", "application/json");
    request->addHeader("Content-Length", std::to_string(info.length()));
    request->put(endpoint, info);
    createTimerForRequest(request);
}

DatabaseManager::DatabaseManager(const std::string& name, int port)
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

} // namespace Http::Asio