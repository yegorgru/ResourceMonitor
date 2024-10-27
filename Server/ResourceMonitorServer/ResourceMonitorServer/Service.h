#pragma once

#include <memory>
#include <string>

#include <boost/asio.hpp>

namespace ResourceMonitorServer {

class Service : public std::enable_shared_from_this<Service> {
public:
    using TcpSocketPtr = std::shared_ptr<boost::asio::ip::tcp::socket>;
public:
    Service(TcpSocketPtr socket);
public:
    void startHandling();
private:
    using RequestBuf = boost::asio::streambuf;
    using Error = boost::system::error_code;
private:
    void onRequestReceived(const Error& ec, std::size_t bytesTransferred);
    void onResponseSent(const Error& ec, std::size_t bytesTransferred);
    void onFinish();
    std::string processRequest(RequestBuf& request);
private:
    using ServicePtr = std::shared_ptr<Service>;
private:
    ServicePtr mSelfPtr;
    TcpSocketPtr mSocket;
    std::string mResponse;
    RequestBuf mRequestBuf;
};

} // namespace ResourceMonitorServer