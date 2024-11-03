#include "Acceptor.h"
#include "Log.h"

namespace ResourceMonitorServer {

Acceptor::Acceptor(IoService& ios)
    : mIos(ios)
    , mAcceptor(nullptr)
    , mIsStopped(false)
{
}

void Acceptor::start(const std::string& rawIp, Port portNum) {
    LOG::Info("Start Acceptor");
    mAcceptor = std::make_unique<TcpAcceptor>(mIos, boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string(rawIp), portNum));
    mAcceptor->listen();
    initAccept();
}

void Acceptor::stop() {
    LOG::Info("Stop Acceptor");
    mIsStopped = true;
}

void Acceptor::initAccept() {
    LOG::Debug("Waiting for new connection");
    auto socket = std::make_shared<boost::asio::ip::tcp::socket>(mIos);

    mAcceptor->async_accept(*socket.get(), 
        [this, socket](const boost::system::error_code& ec) {
            if (ec.value() == 0) {
                LOG::Debug("New connection accepted");
                auto service = std::make_shared<Service>(socket);
                service->startHandling();
            }
            else {
                LOG::Error(LOG::makeLogMessage("Error occured!Error code = ", ec.value(), ". Message: ", ec.message()));
            }

            if (!mIsStopped) {
                initAccept();
            }
            else {
                mAcceptor->close();
            }
        }
    );
}

} // namespace ResourceMonitorServer