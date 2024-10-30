#include "Client.h"

namespace ResourceMonitorClient {

Client::Client()
    : mWork(boost::asio::make_work_guard(mIoService))
    , mThread([this]() { mIoService.run(); })
{
}

Client::HttpRequestPtr Client::createRequest(Http::Request::Id id)
{
    return std::make_shared<Http::Request>(mIoService, id);
}

void Client::close() {
    mWork.reset();
    mThread.join();
}

} // namespace ResourceMonitorClient