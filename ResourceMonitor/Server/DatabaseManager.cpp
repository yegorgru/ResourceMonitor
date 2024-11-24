#include "DatabaseManager.h"
#include "IoService.h"
#include "Log.h"

namespace ResourceMonitorServer {

void DatabaseManager::get(const std::string& endpoint, Http::Request::Callback callback) const {
    auto request = std::make_shared<Http::Request>(IoService::Get().getIoService(), mName, mPort, callback);
    request->get(endpoint);
}

void DatabaseManager::put(const std::string& endpoint, const std::string& info, Http::Request::Callback callback) {
    auto request = std::make_shared<Http::Request>(IoService::Get().getIoService(), mName, mPort, callback);
    LOG::Debug(LOG::composeMessage("Sending info to database:", info));
    request->put(endpoint, info);
}

DatabaseManager::DatabaseManager(const std::string& name, int port)
    : mName(name)
    , mPort(port)
{
}

} // namespace ResourceMonitorServer