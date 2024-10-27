#pragma once

#include <boost/asio.hpp>

namespace ResourceMonitorClient {

struct Session {
public:
    using IoService = boost::asio::io_service;
    using Callback = std::function<void(unsigned int, const std::string&, const boost::system::error_code&)>;
    using Port = unsigned int;
    using Id = unsigned int;
    using TcpSocket = boost::asio::ip::tcp::socket;
    using Endpoint = boost::asio::ip::tcp::endpoint;
    using ResponseBuf = boost::asio::streambuf;
    using ErrorCode = boost::system::error_code;
public:
    Session(IoService& ios, const std::string& rawIpAddress, Port port_num, const std::string& request, Id id, Callback callback);

public:
    TcpSocket mSocket;
    Endpoint mRemoteEndpoint;
    std::string mRequest;

    ResponseBuf mResponseBuf;
    std::string mResponse;

    ErrorCode mErrorCode;

    Id mId;

    Callback mCallback;

    bool mWasCancelled;
    std::mutex mCancelGuard;
};

} // namespace ResourceMonitorClient