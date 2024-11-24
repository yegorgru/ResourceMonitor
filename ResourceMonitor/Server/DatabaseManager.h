#pragma once

#include "Singleton.h"
#include "HttpRequest.h"

#include <string>

namespace ResourceMonitorServer {

class DatabaseManager : public Singleton<DatabaseManager>
{
    friend class Singleton<DatabaseManager>;
public:
    void get(const std::string& endpoint, Http::Request::Callback callback) const;
    void put(const std::string& endpoint, const std::string& info, Http::Request::Callback callback);
private:
    DatabaseManager(const std::string& name, int port);
private:
    std::string mName;
    int mPort;
};

} // namespace ResourceMonitorServer