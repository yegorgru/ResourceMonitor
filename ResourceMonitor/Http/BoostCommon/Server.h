#pragma once

#include <vector>

#include <boost/asio.hpp>

#include "Acceptor.h"
#include "IoService.h"
#include "IServer.h"
#include "DatabaseManager.h"

namespace Http::Boost::Common {

template <typename IoServiceType, typename ServiceType>
class Server : public IServer {
public:
    Server() = default;
    ~Server();
public:
    void start(Port portNum, unsigned int threadPoolSize) override;
    void stop() override;
    void configureDatabase(const std::string& dbName, int dbPort) override;
private:
    using WorkOptional = std::optional<boost::asio::executor_work_guard<boost::asio::io_context::executor_type>>;
    using ThreadPool = std::vector<std::jthread>;
    using AcceptorOptional = std::optional<Acceptor<IoServiceType, ServiceType>>;
private:
    WorkOptional mWork;
    AcceptorOptional mAcceptor;
    ThreadPool mThreadPool;
};

template <typename IoServiceType, typename ServiceType>
Server<IoServiceType, ServiceType>::~Server()
{
    Log::Debug("Destroying server");
    if (mWork && mWork->owns_work()) {
        stop();
    }
}

template <typename IoServiceType, typename ServiceType>
void Server<IoServiceType, ServiceType>::start(Port portNum, unsigned int threadPoolSize) {
    Log::Info("Start server");
    IoService<IoServiceType>::Init();
    if (threadPoolSize < 2) {
        Log::Throw("threadPoolSize should be > 1");
    }
    mWork.emplace(boost::asio::make_work_guard(IoService<IoServiceType>::Get().getIoService()));
    mAcceptor.emplace(IoService<IoServiceType>::Get().getIoService());
    mAcceptor->start("127.0.0.1", portNum);
    mThreadPool.reserve(threadPoolSize);
    for (unsigned int i = 0; i < threadPoolSize; i++) {
        mThreadPool.emplace_back(
            [this]() {
                IoService<IoServiceType>::Get().getIoService().run();
            }
        );
    }

    Log::Info("Server started");
}

template <typename IoServiceType, typename ServiceType>
void Server<IoServiceType, ServiceType>::stop() {
    Log::Info("Stop server");
    if (mAcceptor) {
        mAcceptor->stop();
        mAcceptor.reset();
    }
    if (mWork) {
        mWork->reset();
        mWork.reset();
    }
    for (size_t i = 0; i < mThreadPool.size(); ++i) {
        mThreadPool[i].join();
    }
    mThreadPool.clear();
    IoService<IoServiceType>::Destroy();
    Log::Debug("Server stopped");
}

template <typename IoServiceType, typename ServiceType>
void Server<IoServiceType, ServiceType>::configureDatabase(const std::string& dbName, int dbPort) {
    using DatabaseManager = DatabaseManager<IoServiceType, ServiceType::RequestType>;
    DatabaseManager::Destroy();
    DatabaseManager::Init(dbName, dbPort);
}

} // namespace Http::Boost::Common