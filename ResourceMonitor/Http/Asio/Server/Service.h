#pragma once

#include "HttpMessage.h"

#include <memory>
#include <string>
#include <map>

#include <boost/asio.hpp>

namespace Http::Asio {

class Service;
using ServicePtr = std::shared_ptr<Service>;

class Service : public std::enable_shared_from_this<Service> {
public:
    using TcpSocket = boost::asio::ip::tcp::socket;
    using TcpSocketPtr = std::shared_ptr<TcpSocket>;
public:
    Service(TcpSocketPtr socket);
public:
    void startHandling();
    void sendResponse(StatusCode statusCode, const std::string& response);
private:
    void processRequestLine();
    void processHeadersAndContent();
    void finish();
private:
    using RequestBuf = boost::asio::streambuf;
private:
    ServicePtr mSelfPtr;
    TcpSocketPtr mSocket;
    RequestBuf mRequestBuf;
    MessageRequest mRequest;
    MessageResponse mResponse;
};

} // namespace Http::Asio