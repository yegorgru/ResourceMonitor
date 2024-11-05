#pragma once

#include <functional>
#include <map>
#include <iostream>
#include <memory>

#include <boost/asio.hpp>

#include "HttpRequest.h"

namespace ResourceMonitorClient {

class Client {
private:
    using TcpSocket = boost::asio::ip::tcp::socket;
    using HttpRequestPtr = std::shared_ptr<Http::Request>;
public:
    Client();
    ~Client();
public:
    void makeRequest();
    void cancelRequest();
    void close();
private:
    using IoService = boost::asio::io_service;
    using Work = boost::asio::executor_work_guard<boost::asio::io_context::executor_type>;
private:
    IoService mIoService;
    Work mWork;
    std::thread mThread;
    HttpRequestPtr mCurrentRequest;
};

} // namespace ResourceMonitorClient