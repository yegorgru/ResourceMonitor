#pragma once

#include <string>
#include <map>

#include <boost/asio.hpp>

namespace Http {

class Response {
public:
    using HttpCode = unsigned int;
    using ResponseBuf = boost::asio::streambuf;
    using ResponseStream = std::istream;
    using HttpHeaders = std::map<std::string, std::string>;
public:
    Response();
public:
    unsigned int getStatusCode() const;
    const std::string& getStatusMessage() const;
    const HttpHeaders& getHeaders();
    const ResponseStream& getResponse() const;
    ResponseBuf& getResponseBuf();
    void setStatusCode(unsigned int status_code);
    void setStatusMessage(const std::string& status_message);
    void setHeader(const std::string& name, const std::string& value);
private:
    HttpCode mStatusCode;
    std::string mStatusMessage;
    HttpHeaders mHeaders;
    ResponseBuf mResponseBuf;
    ResponseStream mResponseStream;
};

} // namespace Http