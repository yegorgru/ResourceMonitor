#include "Server.h"

#include "Log.h"
#include "IoService.h"

#include <iostream>

namespace ResourceMonitorServer {

Server::~Server()
{
    Log::Debug("Destroying server");
    if (mWork && mWork->owns_work()) {
        stop();
    }
}

void Server::start(Port portNum, unsigned int threadPoolSize) {
    Log::Info("Start server");
    IoService::Init();
    if (threadPoolSize < 2) {
        Log::Throw("threadPoolSize should be > 1");
    }
    mWork.emplace(boost::asio::make_work_guard(IoService::Get().getIoService()));
    mAcceptor.emplace(IoService::Get().getIoService());
    mAcceptor->start("127.0.0.1", portNum);
    for (unsigned int i = 0; i < threadPoolSize; i++) {
        mThreadPool.emplace_back(
            [this]() {
                IoService::Get().getIoService().run();
            }
        );
    }

    Log::Info("Server started");
}

void Server::stop() {
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
    IoService::Destroy();
    Log::Debug("Server stopped");
}

} // namespace ResourceMonitorServer