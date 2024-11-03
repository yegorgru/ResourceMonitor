#pragma once

#include <atomic>
#include <memory>

#include <boost/asio.hpp>

#include "Service.h"

namespace ResourceMonitorServer {

class Acceptor {
public:
    using IoService = boost::asio::io_service;
    using Port = unsigned int;
public:
    Acceptor(IoService& ios);
public:
    void start(const std::string& rawIp, Port portNum);
    void stop();
private:
    using TcpAcceptor = boost::asio::ip::tcp::acceptor;
    using TcpAcceptorPtr = std::unique_ptr<TcpAcceptor>;
    using AtomicFlag = std::atomic<bool>;
private:
    void initAccept();
private:
    IoService& mIos;
    TcpAcceptorPtr mAcceptor;
    AtomicFlag mIsStopped;
};

} // namespace ResourceMonitorServer