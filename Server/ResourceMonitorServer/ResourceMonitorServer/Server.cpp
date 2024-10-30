#include "Server.h"

namespace ResourceMonitorServer {

Server::Server(unsigned short portNum)
    : mWork(boost::asio::make_work_guard(mIoService))
    , mAcceptor(mIoService, portNum)
{
}

void Server::start(unsigned int thread_pool_size) {
    assert(thread_pool_size > 0);
    mAcceptor.start();
    for (unsigned int i = 0; i < thread_pool_size; i++) {
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