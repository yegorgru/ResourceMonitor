#pragma once

#include "HttpResponse.h"
#include "HttpMethod.h"

#include <iostream>
#include <functional>

#include <boost/asio.hpp>

namespace Http {

class Request;
using RequestPtr = std::shared_ptr<Request>;

class Request : public std::enable_shared_from_this<Request> {
public:
    using IoService = boost::asio::io_service;
    using Port = unsigned int;
    using Callback = std::function<void(Response&)>;
public:
    static const Port DEFAULT_PORT = 80;
public:
    Request(IoService& ios, Callback callback = [](Response&) {});
public:
    void setMethod(Method method);
    void setHost(const std::string& host);
    std::string getHost() const;
    void setPort(Port port);
    Port getPort() const;
    void setUri(const std::string& uri);
    const std::string& getUri() const;
public:
    void execute();
    void cancel();
private:
    void connect(boost::asio::ip::tcp::resolver::iterator iterator);
    void sendRequest();
    void readResponse();
    void readStatusLine();
    void readHeaders();
    void finish(const boost::system::error_code& ec);
private:
    using TcpSocket = boost::asio::ip::tcp::socket;
    using TcpResolver = boost::asio::ip::tcp::resolver;
    using AtomicFlag = std::atomic<bool>;
private:
    RequestPtr mSelfPtr;

    Method mMethod;
    std::string mHost;
    unsigned int mPort;
    std::string mUri;

    Callback mCallback;

    std::string mRequestBuf;

    TcpSocket mSock;
    TcpResolver mResolver;

    Response mResponse;

    AtomicFlag mWasCanceled;

    IoService& mIoService;
};

} // namespace Http