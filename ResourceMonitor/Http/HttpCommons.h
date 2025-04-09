#pragma once

#include <boost/uuid/uuid.hpp>
#include <nlohmann/json.hpp>

#include <functional>
#include <string>
#include <optional>

namespace Http::Commons {

using Id = boost::uuids::uuid;
using PrintCallback = std::function<void(const nlohmann::json& parsedJson)>;
using OptionalPrintCallback = std::optional<PrintCallback>;

Id generateId();
Id stringToId(const std::string& idString);
OptionalPrintCallback getPrintCallback(const std::string& resource);

} // namespace Http::Commons