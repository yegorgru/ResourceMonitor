#pragma once

#include <functional>
#include <map>
#include <iostream>
#include <memory>
#include <mutex>
#include <optional>

#include <boost/asio.hpp>

#include "IClient.h"
#include "HttpRequest.h"

namespace Http::Asio {

class Client : public IClient {
private:
    using TcpSocket = boost::asio::ip::tcp::socket;
    using RequestPtr = std::shared_ptr<Request>;
    using OptionalRequestId = std::optional<std::string>;
public:
    Client();
    ~Client();
public:
    OptionalRequestId makeRequest(int serverPort, const std::string& serverName, const std::string& resource, const std::string& count, const std::string& ipAddress) override;
    void cancelRequest(const std::string strId) override;
    void close() override;
private:
    using IoService = boost::asio::io_service;
    using Work = boost::asio::executor_work_guard<boost::asio::io_context::executor_type>;
    using RequestStorage = std::map<Request::Id, RequestPtr>;
    using OptionalCallback = std::optional<Request::Callback>;
private:
    OptionalCallback getCallback(const std::string& resource);
private:
    IoService mIoService;
    Work mWork;
    std::jthread mThreadIo;
    std::mutex mRequestsMutex;
    RequestStorage mRequests;
};

} // namespace Http::Asio