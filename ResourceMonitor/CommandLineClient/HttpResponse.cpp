#include "HttpResponse.h"

namespace ResourceMonitorClient::Http {

Response::Response()
    : mStatusCode(200)
    , mResponseStream(&mResponseBuf)
{
}

Response::HttpCode Response::getStatusCode() const {
    return mStatusCode;
}

const std::string& Response::getStatusMessage() const {
    return mStatusMessage;
}

const Response::HttpHeaders& Response::getHeaders() {
    return mHeaders;
}

const Response::ResponseStream& Response::getResponse() const {
    return mResponseStream;
}

Response::ResponseBuf& Response::getResponseBuf() {
    return mResponseBuf;
}

void Response::setStatusCode(unsigned int status_code) {
    mStatusCode = status_code;
}

void Response::setStatusMessage(const std::string& status_message) {
    mStatusMessage = status_message;
}

void Response::setHeader(const std::string& name, const std::string& value) {
    mHeaders[name] = value;
}

} // namespace ResourceMonitorClient::Http