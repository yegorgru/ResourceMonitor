#pragma once

#include <functional>
#include <map>
#include <iostream>

#include <boost/asio.hpp>

#include "Session.h"

namespace ResourceMonitorClient {

class Client {
private:
    using TcpSocket = boost::asio::ip::tcp::socket;
public:
    Client();
public:
    void emulateLongComputationOp(unsigned int durationSec, const std::string& rawIpAddress, Session::Port portNum, Session::Id requestId);
    void cancelRequest(Session::Id requestId);
    void close();
private:
    using IoService = boost::asio::io_service;
    using Work = boost::asio::io_service::work;
    using SessionPtr = std::shared_ptr<Session>;
    using SessionStorage = std::map<int, SessionPtr>;
    using ErrorCode = boost::system::error_code;
private:
    void onRequestComplete(SessionPtr session);
    void writeRequest(const ErrorCode& ec, SessionPtr session);
    void readResponse(const ErrorCode& ec, size_t bytesTransferred, SessionPtr session);
    void handler(Session::Id request_id, const std::string& response, const Session::ErrorCode& ec);
private:
    IoService mIoService;
    SessionStorage mActiveSessions;
    std::mutex mActiveSessionsGuard;
    Work mWork;
    std::thread mThread;
};

} // namespace ResourceMonitorClient