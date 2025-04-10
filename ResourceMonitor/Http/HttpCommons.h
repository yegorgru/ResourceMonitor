#pragma once

#include <nlohmann/json.hpp>

#include <functional>
#include <string>
#include <optional>

namespace Http::Commons {

using PrintCallback = std::function<void(const nlohmann::json& parsedJson)>;
using OptionalPrintCallback = std::optional<PrintCallback>;
OptionalPrintCallback getPrintCallback(const std::string& resource);

} // namespace Http::Commons