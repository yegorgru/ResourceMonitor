#pragma once

#include <memory>
#include <string>
#include <map>

#include <boost/asio.hpp>

namespace ResourceMonitorServer {

class Service : public std::enable_shared_from_this<Service> {
public:
    using TcpSocket = boost::asio::ip::tcp::socket;
    using TcpSocketPtr = std::shared_ptr<TcpSocket>;
    using HttpCode = unsigned int;
public:
    Service(TcpSocketPtr socket);
public:
    void startHandling();
private:
    void processRequestLine();
    void processHeadersAndContent();
    void processRequest();
    void sendResponse();
    const std::string& getStatusLine(HttpCode code);
    void finish();
private:
    using RequestBuf = boost::asio::streambuf;
    using ErrorCode = boost::system::error_code;
    using ServicePtr = std::shared_ptr<Service>;
    using HttpHeaders = std::map<std::string, std::string>;
private:
    ServicePtr mSelfPtr;
    TcpSocketPtr mSocket;
    std::string mResponse;
    RequestBuf mRequestBuf;
    HttpHeaders mRequestHeaders;
    std::string mMachineName;
    HttpCode mResponseStatusCode;
    HttpHeaders mResponseHeaders;
    std::string mRequestMethod;
};

} // namespace ResourceMonitorServer