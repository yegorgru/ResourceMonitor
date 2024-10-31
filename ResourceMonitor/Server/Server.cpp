#include "Server.h"

namespace ResourceMonitorServer {

Server::Server(Port portNum)
    : mWork(boost::asio::make_work_guard(mIoService))
    , mAcceptor(mIoService, portNum)
{
}

void Server::start(unsigned int threadPoolSize) {
    assert(threadPoolSize > 0);
    mAcceptor.start();
    for (unsigned int i = 0; i < threadPoolSize; i++) {
        mThreadPool.emplace_back(
            [this]() {
                mIoService.run();
            }
        );
    }
}

void Server::stop() {
    mAcceptor.stop();
    mWork.reset();
    for (auto& th : mThreadPool) {
        th.join();
    }
}

} // namespace ResourceMonitorServer