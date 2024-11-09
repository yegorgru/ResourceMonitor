#include "Client.h"
#include "Log.h"
#include "JsonAdapter.h"

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

void Client::makeRequest() {
    LOG::Debug("Making request");
    static auto clientCallback = [](Http::Response& response) {
        auto machineState = JsonAdapter::jsonToMachineState(response.getBody());

        if (machineState) {
            LOG::Info(LOG::makeLogMessage("Name:", machineState->mName));
            LOG::Info(LOG::makeLogMessage("CpuUsage:", machineState->mCpuUsage));
            LOG::Info(LOG::makeLogMessage("MemoryUsage:", machineState->mMemoryUsage));
            LOG::Info(LOG::makeLogMessage("TotalMemory:", machineState->mTotalMemory));
            LOG::Info(LOG::makeLogMessage("MemoryUsed:", machineState->mMemoryUsed));
            LOG::Info(LOG::makeLogMessage("DiskUsage:", machineState->mDiskUsage));
            LOG::Info(LOG::makeLogMessage("TotalDisk:", machineState->mTotalDisk));
            LOG::Info(LOG::makeLogMessage("DiskUsed:", machineState->mDiskUsed));
        }
        else {
            LOG::Error("machine not found");
        }

        auto message = LOG::makeLogMessage("Request processed successfully");
        LOG::Info(message);
        std::cout << message << std::endl;
    };
    auto request = std::make_shared<Http::Request>(mIoService, clientCallback);
    request->setHost("localhost");
    request->setUri("/index.html");
    request->setPort(8080);
    request->execute();
}

void Client::cancelRequest() {
    LOG::Debug("Canceling request");
}

void Client::close() {
    LOG::Debug("Client close");
    mWork.reset();
    mThread.join();
}

} // namespace ResourceMonitorClient