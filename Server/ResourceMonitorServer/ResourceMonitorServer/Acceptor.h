#pragma once

#include <atomic>

#include <boost/asio.hpp>

#include "Service.h"

namespace ResourceMonitorServer {

class Acceptor {
public:
    using IoService = boost::asio::io_service;
    using TcpAcceptor = boost::asio::ip::tcp::acceptor;
    using Port = unsigned int;
public:
    Acceptor(IoService& ios, Port portNum);
public:
    void start();
    void stop();
private:
    using AtomicFlag = std::atomic<bool>;
    using Error = boost::system::error_code;
    using TcpSocket = boost::asio::ip::tcp::socket;
    using TcpSocketPtr = std::shared_ptr<TcpSocket>;
private:
    void initAccept();
    void onAccept(const Error& ec, TcpSocketPtr sock);
private:
    IoService& mIos;
    TcpAcceptor mAcceptor;
    AtomicFlag mIsStopped;
};

} // namespace ResourceMonitorServer