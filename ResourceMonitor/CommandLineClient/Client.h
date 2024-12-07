#pragma once

#include <functional>
#include <map>
#include <iostream>
#include <memory>
#include <mutex>
#include <optional>

#include <boost/asio.hpp>

#include "HttpRequest.h"

namespace ResourceMonitorClient {

class Client {
private:
    using TcpSocket = boost::asio::ip::tcp::socket;
    using HttpRequestPtr = std::shared_ptr<Http::Request>;
    using OptionalRequestId = std::optional<std::string>;
public:
    Client();
    ~Client();
public:
    OptionalRequestId makeRequest(int serverPort, const std::string& serverName, const std::string& resource, const std::string& count, const std::string& ipAddress);
    void cancelRequest(const std::string strId);
    void close();
private:
    using IoService = boost::asio::io_service;
    using Work = boost::asio::executor_work_guard<boost::asio::io_context::executor_type>;
    using RequestStorage = std::map<Http::Request::Id, Http::RequestPtr>;
    using OptionalCallback = std::optional<Http::Request::Callback>;
private:
    OptionalCallback getCallback(const std::string& resource);
private:
    IoService mIoService;
    Work mWork;
    std::jthread mThreadIo;
    std::mutex mRequestsMutex;
    RequestStorage mRequests;
};

} // namespace ResourceMonitorClient