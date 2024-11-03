#include "Server.h"
#include "Log.h"

namespace ResourceMonitorServer {

Server::Server(Port portNum)
    : mWork(boost::asio::make_work_guard(mIoService))
    , mAcceptor(mIoService, portNum)
{
}

void Server::start(unsigned int threadPoolSize) {
    LOG::Info("Start server");
    if (threadPoolSize == 0) {
        LOG::Throw("threadPoolSize should be > 0");
    }
    mAcceptor.start();
    for (unsigned int i = 0; i < threadPoolSize; i++) {
        mThreadPool.emplace_back(
            [this]() {
                mIoService.run();
            }
        );
    }
    LOG::Debug("Server started");
}

void Server::stop() {
    LOG::Info("Stop server");
    mAcceptor.stop();
    mWork.reset();
    for (auto& th : mThreadPool) {
        th.join();
    }
    LOG::Debug("Server stopped");
}

} // namespace ResourceMonitorServer