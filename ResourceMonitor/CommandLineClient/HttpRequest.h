#pragma once

#include <iostream>

#include <boost/asio.hpp>

#include "HttpResponse.h"

namespace ResourceMonitorClient::Http {

class Request {
public:
    using IoService = boost::asio::io_service;
    using Id = unsigned int;
    using Port = unsigned int;
public:
    static const Port DEFAULT_PORT = 80;
public:
    Request(IoService& ios, Id id);
public:
    Id getId() const;
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
    std::string mHost;
    unsigned int mPort;
    std::string mUri;

    Id mId;

    std::string mRequestBuf;

    TcpSocket mSock;
    TcpResolver mResolver;

    Response mResponse;

    AtomicFlag mWasCanceled;

    IoService& mIoService;
};

} // namespace ResourceMonitorClient::Http