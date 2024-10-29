#include "Client.h"

namespace ResourceMonitorClient {

Client::Client()
    : mWork(mIoService)
    , mThread([this]() { mIoService.run(); })
{
}

Client::HttpRequestPtr Client::createRequest(Http::Request::Id id)
{
    return std::make_shared<Http::Request>(mIoService, id);
}

void Client::close() {
    mIoService.stop();
    mThread.join();
}

} // namespace ResourceMonitorClient