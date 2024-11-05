#include "Server.h"

#include "Log.h"
#include "JsonAdapter.h"
#include "MachineState.h"

#include <iostream>

namespace ResourceMonitorServer {

Server::Server()
    : mWork(boost::asio::make_work_guard(mIoService))
    , mAcceptor(mIoService)
{
}

void Server::start(Port portNum, unsigned int threadPoolSize) {
    LOG::Info("Start server");
    if (threadPoolSize < 2) {
        LOG::Throw("threadPoolSize should be > 1");
    }
    mAcceptor.start("127.0.0.1", portNum);
    for (unsigned int i = 0; i < threadPoolSize; i++) {
        mThreadPool.emplace_back(
            [this]() {
                mIoService.run();
            }
        );
    }

    LOG::Info("Server started");
}

void Server::stop() {
    LOG::Info("Stop server");
    mAcceptor.stop();
    mWork.reset();
    for (size_t i = 0; i < mThreadPool.size(); ++i) {
        mThreadPool[i].join();
    }
    LOG::Debug("Server stopped");
}

} // namespace ResourceMonitorServer