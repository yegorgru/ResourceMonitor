#pragma once

#include <vector>

#include <boost/asio.hpp>

#include "IServer.h"
#include "Acceptor.h"

namespace Http::Asio {

class Server : public IServer {
public:
    using Port = unsigned int;
public:
    Server() = default;
    ~Server();
public:
    void start(Port portNum, unsigned int threadPoolSize) override;
    void stop() override;
    void configureDatabase(const std::string& dbName, int dbPort) override;
private:
    using WorkOptional = std::optional<boost::asio::executor_work_guard<boost::asio::io_context::executor_type>>;
    using ThreadPool = std::vector<std::thread>;
    using AcceptorOptional = std::optional<Acceptor>;
private:
    WorkOptional mWork;
    AcceptorOptional mAcceptor;
    ThreadPool mThreadPool;
};

} // namespace Http::Asio