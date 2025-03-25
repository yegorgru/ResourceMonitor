#pragma once

#include <vector>

#include <boost/asio.hpp>

#include "Acceptor.h"
#include "DatabaseManager.h"

namespace ResourceMonitorServer {

class Server {
public:
    using Port = unsigned int;
public:
    Server() = default;
    ~Server();
public:
    void start(Port portNum, unsigned int threadPoolSize);
    void stop();
private:
    using WorkOptional = std::optional<boost::asio::executor_work_guard<boost::asio::io_context::executor_type>>;
    using ThreadPool = std::vector<std::thread>;
    using AcceptorOptional = std::optional<Acceptor>;
private:
    WorkOptional mWork;
    AcceptorOptional mAcceptor;
    ThreadPool mThreadPool;
};

} // namespace ResourceMonitorServer