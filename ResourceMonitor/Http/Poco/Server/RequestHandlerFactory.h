#pragma once

#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPServerRequest.h>

namespace Http::Poco {

class RequestHandlerFactory : public ::Poco::Net::HTTPRequestHandlerFactory {
public:
    RequestHandlerFactory();
    ~RequestHandlerFactory() = default;
public:
    using RequestRawPtr = ::Poco::Net::HTTPRequestHandler*;
    RequestRawPtr createRequestHandler(const ::Poco::Net::HTTPServerRequest& request) override;
};

} // namespace Http::Poco
