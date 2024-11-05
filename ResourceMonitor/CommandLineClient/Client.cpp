#include "Client.h"
#include "Log.h"

namespace ResourceMonitorClient {

Client::Client()
    : mWork(boost::asio::make_work_guard(mIoService))
    , mThread([this]() { mIoService.run(); })
{
}

Client::~Client()
{
    LOG::Debug("Destroying client");
    if (mWork.owns_work()) {
        close();
    }
}

void Client::makeRequest(Http::Request::Id id)
{
    LOG::Debug("Making request");
    if (mCurrentRequest && !mCurrentRequest->isCompleted()) {
        LOG::Warning("Client has active request");
        return;
    }
    mCurrentRequest = std::make_shared<Http::Request>(mIoService, id);
    mCurrentRequest->setHost("localhost");
    mCurrentRequest->setUri("/index.html");
    mCurrentRequest->setPort(3333);
    mCurrentRequest->execute();
}

void Client::close() {
    LOG::Debug("Client close");
    mWork.reset();
    mThread.join();
}

} // namespace ResourceMonitorClient