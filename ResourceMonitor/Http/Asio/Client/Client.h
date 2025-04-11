#pragma once

#include <functional>
#include <map>
#include <iostream>
#include <memory>
#include <mutex>
#include <optional>

#include <boost/asio.hpp>

#include "IClient.h"
#include "Asio/HttpRequest.h"

namespace Http::Asio {

class Client : public IClient {
public:
    Client();
    ~Client();
public:
    OptionalRequestId makeRequest(Port serverPort, const std::string& serverName, const std::string& resource, const std::string& count, const std::string& ipAddress) override;
    void cancelRequest(const std::string strId) override;
    void close() override;
private:
    using IoService = boost::asio::io_service;
    using Work = boost::asio::executor_work_guard<boost::asio::io_context::executor_type>;
    using RequestStorage = std::map<Id, RequestPtr>;
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