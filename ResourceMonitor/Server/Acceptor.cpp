#include "Acceptor.h"
#include "Log.h"

namespace ResourceMonitorServer {

Acceptor::Acceptor(IoService& ios, Port portNum)
    : mIos(ios)
    , mAcceptor(mIos, boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), portNum))
    , mIsStopped(false)
{
}

void Acceptor::start() {
    LOG::Info("Start Acceptor");
    mAcceptor.listen();
    initAccept();
}

void Acceptor::stop() {
    LOG::Info("Stop Acceptor");
    mIsStopped = true;
}

void Acceptor::initAccept() {
    LOG::Debug("Waiting for new connection");
    auto socket = std::make_shared<boost::asio::ip::tcp::socket>(mIos);

    mAcceptor.async_accept(*socket.get(), 
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
                mAcceptor.close();
            }
        }
    );
}

} // namespace ResourceMonitorServer