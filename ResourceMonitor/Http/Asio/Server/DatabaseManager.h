#pragma once

#include "Singleton.h"
#include "HttpRequest.h"

#include <string>

namespace Http::Asio {

class DatabaseManager : public Singleton<DatabaseManager>
{
    friend class Singleton<DatabaseManager>;
public:
    void get(const std::string& endpoint, Request::Callback callback) const;
    void put(const std::string& endpoint, const std::string& info, Request::Callback callback);
private:
    DatabaseManager(const std::string& name, int port);
private:
    std::string mName;
    int mPort;
};

} // namespace Http::Asio