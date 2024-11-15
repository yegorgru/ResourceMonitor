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

void Client::makeRequest(int serverPort, const std::string& serverName) {
    LOG::Debug("Making request");
    static auto clientCallback = [](Http::MessageResponse& response) {
        auto statusCode = response.getStatusCode();
        std::string message;
        if (statusCode == 200) {
            auto machineState = JsonAdapter::jsonToMachineState(response.getBody());
            if (machineState) {
                LOG::Info(LOG::composeMessage("Name:", machineState->mName));
                LOG::Info(LOG::composeMessage("CpuUsage:", machineState->mCpuUsage));
                LOG::Info(LOG::composeMessage("MemoryUsage:", machineState->mMemoryUsage));
                LOG::Info(LOG::composeMessage("TotalMemory:", machineState->mTotalMemory));
                LOG::Info(LOG::composeMessage("MemoryUsed:", machineState->mMemoryUsed));
                LOG::Info(LOG::composeMessage("DiskUsage:", machineState->mDiskUsage));
                LOG::Info(LOG::composeMessage("TotalDisk:", machineState->mTotalDisk));
                LOG::Info(LOG::composeMessage("DiskUsed:", machineState->mDiskUsed));

                message = "Request processed successfully";
                LOG::Info(message);
            }
            else {
                message = LOG::composeMessage("Failed to parse machineState", response.getBody());
                LOG::Error(message);
            }
        }
        else {
            message = LOG::composeMessage("Failed to get machine state from server", statusCode, response.getBody());
            LOG::Error(message);
        }
        LOG::SyncPrintLine(message, std::cout);
    };
    auto request = std::make_shared<Http::Request>(mIoService, serverName, serverPort, clientCallback);
    request->get("machine");
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