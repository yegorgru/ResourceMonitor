#pragma once

#include "Singleton.h"
#include "HttpCommon.h"
#include "Poco/HttpSession.h"

#include <string>
#include <functional>
#include <memory>

namespace Http::Poco {

class DatabaseManager : public Singleton<DatabaseManager>
{
    friend class Singleton<DatabaseManager>;
public:
    void get(const std::string& endpoint, Session::Callback callback) const;
    void put(const std::string& endpoint, const std::string& info, Session::Callback callback);
private:
    DatabaseManager(const std::string& name, Port port);
private:
    std::string mName;
    int mPort;
};

} // namespace Http::Poco
