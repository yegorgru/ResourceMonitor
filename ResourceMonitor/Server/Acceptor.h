#pragma once

#include <atomic>

#include <boost/asio.hpp>

#include "Service.h"

namespace ResourceMonitorServer {

class Acceptor {
public:
    using IoService = boost::asio::io_service;
    using Port = unsigned int;
public:
    Acceptor(IoService& ios, Port portNum);
public:
    void start();
    void stop();
private:
    using TcpAcceptor = boost::asio::ip::tcp::acceptor;
    using AtomicFlag = std::atomic<bool>;
private:
    void initAccept();
private:
    IoService& mIos;
    TcpAcceptor mAcceptor;
    AtomicFlag mIsStopped;
};

} // namespace ResourceMonitorServer