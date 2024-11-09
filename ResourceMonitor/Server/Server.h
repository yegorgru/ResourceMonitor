#pragma once

#include <vector>

#include <boost/asio.hpp>

#include "Acceptor.h"
#include "DatabaseManager.h"

namespace ResourceMonitorServer {

class Server {
public:
    using IoService = boost::asio::io_service;
    using Port = unsigned int;
public:
    Server(IoService& ioService);
    ~Server();
public:
    void start(Port portNum, unsigned int threadPoolSize);
    void stop();
private:
    using Work = boost::asio::executor_work_guard<boost::asio::io_context::executor_type>;
    using ThreadPool = std::vector<std::thread>;
private:
    IoService& mIoService;
    Work mWork;
    Acceptor mAcceptor;
    ThreadPool mThreadPool;
};

} // namespace ResourceMonitorServer