#pragma once

#include <functional>
#include <map>
#include <iostream>
#include <memory>
#include <mutex>
#include <optional>

#include <boost/beast/core.hpp>

#include "IClient.h"
#include "Beast/HttpSession.h"

namespace Http::Beast {

class Client : public IClient {
public:
    using OptionalRequestId = std::optional<std::string>;
public:
    Client();
    ~Client();
public:
    OptionalRequestId makeRequest(int serverPort, const std::string& serverName, const std::string& resource, const std::string& count, const std::string& ipAddress) override;
    void cancelRequest(const std::string strId) override;
    void close() override;
private:
    using OptionalCallback = std::optional<Session::Callback>;
    OptionalCallback getCallback(const std::string& resource);
private:
    using IoContext = boost::asio::io_context;
    using Work = boost::asio::executor_work_guard<boost::asio::io_context::executor_type>;
    using SessionStorage = std::map<Id, SessionPtr>;
private:
    IoContext mIoContext;
    Work mWork;
    std::jthread mThreadIo;
    std::mutex mSessionsMutex;
    SessionStorage mSessions;
};

} // namespace Http::Beast