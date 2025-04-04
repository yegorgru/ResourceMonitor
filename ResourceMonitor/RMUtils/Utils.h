#include <optional>
#include <string>
#include <functional>

std::optional<int> stringToInt(const std::string& value, const std::function<bool(int)>& condition = [](int) { return true; });

bool isValidIpAddress(const std::string& ipAddress);
bool isValidPort(int port);