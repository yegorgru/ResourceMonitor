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
    /*mAcceptor.start("127.0.0.1", portNum);
    for (unsigned int i = 0; i < threadPoolSize; i++) {
        mThreadPool.emplace_back(
            [this]() {
                mIoService.run();
            }
        );
    }*/

    /*return {
        {"name", machineState.mName},
        {"cpu", {
            {"usage %", machineState.mCpuUsage}
        }},
        {"memory", {
            {"usage %", machineState.mMemoryUsage},
            {"total GB", machineState.mTotalMemory},
            {"used GB", machineState.mMemoryUsed}
        }},
        {"disk", {
            {"usage %", machineState.mDiskUsed},
            {"total GB", machineState.mTotalDisk},
            {"used GB", machineState.mDiskUsed}
        }}
    };*/

    const auto& machineState = mDatabaseManager.getMachineState("My Name");
    const auto& jsonObject = JsonAdapter::machineStateToJson(machineState);

    LOG::Info(LOG::makeLogMessage("Get json for machine", jsonObject["name"]));
    LOG::Info(LOG::makeLogMessage("Get cpu usage:", jsonObject["cpu"]["usage %"]));
    LOG::Info(LOG::makeLogMessage("Get memory usage:", jsonObject["memory"]["usage %"]));
    LOG::Info(LOG::makeLogMessage("Get memory total GB:", jsonObject["memory"]["total GB"]));
    LOG::Info(LOG::makeLogMessage("Get memory used GB:", jsonObject["memory"]["used GB"]));
    LOG::Info(LOG::makeLogMessage("Get disk usage:", jsonObject["disk"]["usage %"]));
    LOG::Info(LOG::makeLogMessage("Get disk total GB:", jsonObject["disk"]["total GB"]));
    LOG::Info(LOG::makeLogMessage("Get disk used GB:", jsonObject["disk"]["used GB"]));

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