#pragma once

#include <atomic>
#include <memory>

#include <boost/asio.hpp>

#include "Log.h"
#include "HttpCommon.h"

namespace Http::Boost::Common {

template <typename Context, typename Service>
class Acceptor {
public:
    Acceptor(Context& ios);
    ~Acceptor() = default;
public:
    void start(const std::string& rawIp, Port portNum);
    void stop();
private:
    void initAccept();
private:
    using TcpAcceptor = boost::asio::ip::tcp::acceptor;
    using TcpAcceptorPtr = std::unique_ptr<TcpAcceptor>;
    using AtomicFlag = std::atomic<bool>;
    using TcpSocket = boost::asio::ip::tcp::socket;
    using TcpSocketPtr = std::shared_ptr<TcpSocket>;
private:
    Context& mIos;
    TcpAcceptorPtr mAcceptor;
    TcpSocketPtr mNextSocket;
    AtomicFlag mIsStopped;
};

template <typename Context, typename Service>
Acceptor<Context, Service>::Acceptor(Context& ios)
    : mIos(ios)
    , mAcceptor(nullptr)
    , mIsStopped(false)
{
}

template <typename Context, typename Service>
void Acceptor<Context, Service>::start(const std::string& rawIp, Port portNum) {
    Log::Info("Start Acceptor");
    try {
        mAcceptor = std::make_unique<TcpAcceptor>(mIos, boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string(rawIp), portNum));
        mAcceptor->listen();
        initAccept();
    } catch (const boost::system::system_error& e) {
        Log::Throw(Print::composeMessage("Can't listen on specified ip and port. IP: ", rawIp, "Port:", portNum,". Error: ", e.what()));
    }
}

template <typename Context, typename Service>
void Acceptor<Context, Service>::stop() {
    Log::Info("Stop Acceptor");
    if (mNextSocket) {
        mNextSocket->close();
    }
    if (mAcceptor) {
        mAcceptor->close();
    }
    mIsStopped = true;
}

template <typename Context, typename Service>
void Acceptor<Context, Service>::initAccept() {
    Log::Debug("Waiting for new connection");
    mNextSocket = std::make_shared<TcpSocket>(mIos);

    mAcceptor->async_accept(*mNextSocket.get(),
        [this](const boost::system::error_code& ec) {
            if (ec.value() == 0) {
                Log::Debug("New connection accepted");
                auto service = std::make_shared<Service>(mNextSocket);
                service->startHandling();
            }
            else {
                Log::Error(Print::composeMessage("Error occured! Error code = ", ec.value(), ". Message: ", ec.message()));
            }
            if (!mIsStopped) {
                initAccept();
            }
        }
    );
}

} // namespace Http::Boost::Common