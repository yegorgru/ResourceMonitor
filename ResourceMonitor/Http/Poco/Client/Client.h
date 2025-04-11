#pragma once

#include <functional>
#include <map>
#include <iostream>
#include <memory>
#include <mutex>
#include <optional>
#include <string>

#include "IClient.h"
#include "Utils.h"
#include "Poco/HttpSession.h"

namespace Http::Poco {

class Client : public IClient {
public:
    Client();
    ~Client();
public:
    OptionalRequestId makeRequest(Port serverPort, const std::string& serverName, const std::string& resource, const std::string& count, const std::string& ipAddress) override;
    void cancelRequest(const std::string strId) override;
    void close() override;
private:
    using OptionalCallback = std::optional<Session::Callback>;
    OptionalCallback getCallback(const std::string& resource);
private:
    using SessionStorage = std::map<Id, SessionPtr>;
private:
    std::mutex mSessionsMutex;
    SessionStorage mSessions;
};

} // namespace Http::Poco