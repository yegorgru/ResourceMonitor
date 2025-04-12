#pragma once

#include <memory>
#include <set>

#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>

#include "Utils.h"
#include "HttpCommon.h"

namespace Http::Poco {

class RequestHandler : public ::Poco::Net::HTTPRequestHandler {
public:
    using Request = ::Poco::Net::HTTPServerRequest;
    using Response = ::Poco::Net::HTTPServerResponse;
public:
    RequestHandler();
    ~RequestHandler() = default;
public:
    void handleRequest(Request& request, Response& response) override;
private:
    void processGetRequest(const std::string& resource, const std::string& body, Response& response);
    void processPutRequest(const std::string& resource, const std::string& body, Response& response);
    void sendResponse(Response& response, StatusCode status, const std::string& body = "");
};

} // namespace Http::Poco
