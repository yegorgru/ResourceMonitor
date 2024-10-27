#include "Acceptor.h"

#include <iostream>

namespace ResourceMonitorServer {

Acceptor::Acceptor(IoService& ios, unsigned short port_num)
    : mIos(ios)
    , mAcceptor(mIos, boost::asio::ip::tcp::endpoint(boost::asio::ip::address_v4::any(), port_num)), mIsStopped(false)
{
}

void Acceptor::start() {
    mAcceptor.listen();
    initAccept();
}

void Acceptor::stop() {
    mIsStopped.store(true);
}

void Acceptor::initAccept() {
    auto socket = std::make_shared<TcpSocket>(mIos);

    mAcceptor.async_accept(*socket.get(), 
        [this, socket](
            const boost::system::error_code& error)
        {
            onAccept(error, socket);
        }
    );
}

void Acceptor::onAccept(const Error& ec, TcpSocketPtr sock)
{
    if (ec.value() == 0) {
        (new Service(sock))->startHandling();
    }
    else {
        std::cout << "Error occured! Error code = " << ec.value() << ". Message: " << ec.message();
    }

    if (!mIsStopped.load()) {
        initAccept();
    }
    else {
        mAcceptor.close();
    }
}

} // namespace ResourceMonitorServer