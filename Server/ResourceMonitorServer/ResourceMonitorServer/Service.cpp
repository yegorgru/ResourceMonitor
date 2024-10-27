#include "Service.h"

#include <iostream>

namespace ResourceMonitorServer {

Service::Service(TcpSocketPtr socket)
    : mSocket(socket)
{

}

void Service::startHandling() {
    boost::asio::async_read_until(*mSocket.get(), mRequestBuf, '\n',
        [this](const boost::system::error_code& ec, std::size_t bytes_transferred)
        {
            onRequestReceived(ec, bytes_transferred);
        }
    );
}

void Service::onRequestReceived(const Error& ec, std::size_t bytesTransferred) {
    if (ec.value() != 0) {
        std::cout << "Error occured! Error code = " << ec.value() << ". Message: " << ec.message();

        onFinish();
        return;
    }

    mResponse = processRequest(mRequestBuf);

    boost::asio::async_write(*mSocket.get(), boost::asio::buffer(mResponse),
        [this](
            const boost::system::error_code& ec,
            std::size_t bytes_transferred)
        {
            onResponseSent(ec,
            bytes_transferred);
        }
    );
}

void Service::onResponseSent(const Error& ec, std::size_t bytesTransferred) {
    if (ec.value() != 0) {
        std::cout << "Error occured! Error code = "
            << ec.value()
            << ". Message: " << ec.message();
    }

    onFinish();
}

void Service::onFinish() {
    delete this;
}

std::string Service::processRequest(RequestBuf& request) {
    std::string response = "Response\n";
    return response;
}

} // namespace ResourceMonitorServer