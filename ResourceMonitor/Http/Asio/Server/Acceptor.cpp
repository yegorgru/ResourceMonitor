#include "Acceptor.h"
#include "Log.h"

namespace Http::Asio {

Acceptor::Acceptor(IoService& ios)
    : mIos(ios)
    , mAcceptor(nullptr)
    , mIsStopped(false)
{
}

void Acceptor::start(const std::string& rawIp, Port portNum) {
    Log::Info("Start Acceptor");
    mAcceptor = std::make_unique<TcpAcceptor>(mIos, boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string(rawIp), portNum));
    mAcceptor->listen();
    initAccept();
}

void Acceptor::stop() {
    Log::Info("Stop Acceptor");
    if (mNextSocket) {
        mNextSocket->close();
    }
    mAcceptor->close();
    mIsStopped = true;
}

void Acceptor::initAccept() {
    Log::Debug("Waiting for new connection");
    mNextSocket = std::make_shared<boost::asio::ip::tcp::socket>(mIos);

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

} // namespace Http::Asio