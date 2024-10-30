#pragma once

#include <memory>
#include <vector>

#include <boost/asio.hpp>

#include "Acceptor.h"

namespace ResourceMonitorServer {

class Server {
public:
    Server(unsigned short portNum);
public:
    void start(unsigned int thread_pool_size);
    void stop();
private:
    using IoService = boost::asio::io_service;
    using Work = boost::asio::executor_work_guard<boost::asio::io_context::executor_type>;
    using ThreadPool = std::vector<std::thread>;
private:
    IoService mIoService;
    Work mWork;
    Acceptor mAcceptor;
    ThreadPool mThreadPool;
};

} // namespace ResourceMonitorServer