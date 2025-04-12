#pragma once

#include <nlohmann/json.hpp>

#include <functional>
#include <string>
#include <optional>

namespace Http {

enum class StatusCode {
    Ok = 200, 
    NotFound = 404,
    UnsupportedMediaType = 415,
    RequestEntityTooLarge = 413,
    ClientClosedRequest = 499,
    ServerError = 500,
    NotImplemented = 501,
    HttpVersionNotSupported = 505
};

using Port = unsigned int;
constexpr Port DEFAULT_PORT = 80;
constexpr unsigned int CANCELED_HTTP_STATUS = 499;

using PrintCallback = std::function<void(const nlohmann::json& parsedJson)>;
using OptionalPrintCallback = std::optional<PrintCallback>;
OptionalPrintCallback getPrintCallback(const std::string& resource);

bool isValidEndpoint(const std::string& uri, const std::string& method);

} // namespace Http