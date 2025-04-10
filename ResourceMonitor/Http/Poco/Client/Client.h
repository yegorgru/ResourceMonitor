#pragma once

#include <functional>
#include <map>
#include <iostream>
#include <memory>
#include <mutex>
#include <optional>

#include "IClient.h"

namespace Http::Poco {

class Client : public IClient {
public:
    using OptionalRequestId = std::optional<std::string>;
public:
    Client();
    ~Client();
public:
    OptionalRequestId makeRequest(int serverPort, const std::string& serverName, const std::string& resource, const std::string& count, const std::string& ipAddress) override;
    void cancelRequest(const std::string strId) override;
    void close() override;
};

} // namespace Http::Poco