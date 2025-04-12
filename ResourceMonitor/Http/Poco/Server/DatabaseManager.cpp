#include "DatabaseManager.h"
#include "Log.h"
#include "Print.h"
#include <chrono>
#include <future>

namespace Http::Poco {

DatabaseManager::DatabaseManager(const std::string& name, Port port)
    : mName(name)
    , mPort(port)
{
    Log::Debug(Print::composeMessage("Creating Poco DatabaseManager with name:", name, "port:", port));
}

void DatabaseManager::get(const std::string& endpoint,Session:: Callback callback) const
{
    Log::Debug(Print::composeMessage("Sending GET request to database, endpoint:", endpoint));
    
    auto session = std::make_shared<Session>(mName, mPort, callback);
    session->setTimeout(30);
    session->setUseThreadPool(false);
    session->get(endpoint);
}

void DatabaseManager::put(const std::string& endpoint, const std::string& info, Session::Callback callback)
{
    Log::Debug(Print::composeMessage("Sending PUT request to database, endpoint:", endpoint));
    
    auto session = std::make_shared<Session>(mName, mPort, callback);
    session->addHeader("Content-Type", "application/json");
    session->setTimeout(30);
    session->setUseThreadPool(false);
    session->put(endpoint, info);
}

} // namespace Http::Poco
