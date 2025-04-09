#pragma once

#include <memory>
#include <string>
#include <map>

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>

#include "Beast/HttpSession.h"

namespace Http::Beast {

class Service;
using ServicePtr = std::shared_ptr<Service>;

class Service : public std::enable_shared_from_this<Service> {
public:
    using TcpSocket = boost::asio::ip::tcp::socket;
    using TcpSocketPtr = std::shared_ptr<TcpSocket>;
    using HttpResponse = boost::beast::http::response<boost::beast::http::string_body>;
    using RequestType = Session;
public:
    Service(TcpSocketPtr socket);
public:
    void startHandling();
    void sendResponse(boost::beast::http::status status, const std::string& response);
private:
    void readRequest();
    void prepareResponse();
    void finish();
private:
    using TcpStream = boost::beast::tcp_stream;
    using FlatBuffer = boost::beast::flat_buffer;
    using HttpRequest = boost::beast::http::request<boost::beast::http::string_body>;
private:
    TcpStream mStream;
    FlatBuffer mBuffer;
    HttpRequest mRequest;
    HttpResponse mResponse;
    ServicePtr mSelfPtr;
};

} // namespace Http::Beast