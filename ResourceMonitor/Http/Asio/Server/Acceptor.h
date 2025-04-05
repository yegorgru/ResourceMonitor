#pragma once

#include <atomic>
#include <memory>

#include <boost/asio.hpp>

#include "Service.h"

namespace Http::Asio {

class Acceptor {
public:
    using IoService = boost::asio::io_service;
    using Port = unsigned int;
public:
    Acceptor(IoService& ios);
    ~Acceptor() = default;
public:
    void start(const std::string& rawIp, Port portNum);
    void stop();
private:
    using TcpAcceptor = boost::asio::ip::tcp::acceptor;
    using TcpAcceptorPtr = std::unique_ptr<TcpAcceptor>;
    using AtomicFlag = std::atomic<bool>;
    using TcpSocket = boost::asio::ip::tcp::socket;
    using TcpSocketPtr = std::shared_ptr<TcpSocket>;
private:
    void initAccept();
private:
    IoService& mIos;
    TcpAcceptorPtr mAcceptor;
    TcpSocketPtr mNextSocket;
    AtomicFlag mIsStopped;
};

} // namespace Http::Asio