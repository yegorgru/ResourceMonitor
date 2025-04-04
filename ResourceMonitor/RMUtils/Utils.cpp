#include "Utils.h"

#include <sstream>
#include <algorithm>

std::optional<int> stringToInt(const std::string& value, const std::function<bool(int)>& condition) {
    std::istringstream iss(value);
    int result;
    iss >> result;
    if (iss.fail() || !iss.eof() || !condition(result)) {
        return std::nullopt;
    }
    return result;
}

bool isValidIpAddress(const std::string& ipAddress) {
    std::istringstream iss(ipAddress);
    std::string segment;
    int count = 0;
    while (std::getline(iss, segment, '.')) {
        if (segment.empty() || segment.size() > 3 || !std::all_of(segment.begin(), segment.end(), std::isdigit)) {
            return false;
        }
        int num = std::stoi(segment);
        if (num < 0 || num > 255) {
            return false;
        }
        if (++count > 4) {
            return false;
        }
    }
    return count == 4;
}

bool isValidPort(int port) {
    return port >= 0 && port <= 65535;
}