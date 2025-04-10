#include "Client.h"
#include "Log.h"
#include "HttpCommons.h"

#include <boost/uuid/uuid_io.hpp>

namespace Http::Poco {

Client::Client()
{
    Log::Debug("Creating client");
}

Client::~Client()
{
    Log::Debug("Destroying client");
}

Client::OptionalRequestId Client::makeRequest(int serverPort, const std::string& serverName, const std::string& resource, const std::string& count, const std::string& ipAddress) {
    Log::Debug("Client::makeRequest");
    return std::string("");
}

void Client::cancelRequest(const std::string strId) {
    Log::Debug("Client::cancelRequest");
}

void Client::close() {
    Log::Debug("Client::close");
}

} // namespace Http::Poco