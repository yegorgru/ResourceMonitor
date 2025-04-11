#pragma once

#include <string>
#include <optional>
#include <memory>

#include "HttpCommon.h"

namespace Http {

class IClient {
public:
    IClient() = default;
    virtual ~IClient() = default;
public:
    using OptionalRequestId = std::optional<std::string>;
public:
    virtual OptionalRequestId makeRequest(Port serverPort, const std::string& serverName, const std::string& resource, const std::string& count, const std::string& ipAddress) = 0;
    virtual void cancelRequest(const std::string strId) = 0;
    virtual void close() = 0;
};

using ClientPtr = std::unique_ptr<IClient>;

} // namespace Http