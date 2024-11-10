#include "DatabaseManager.h"
#include "HttpRequest.h"
#include "IoService.h"
#include "Log.h"
#include "JsonAdapter.h"

namespace ResourceMonitorServer {

void DatabaseManager::getMachineState(const std::string& machineName, ServicePtr service) const {
    auto callback = [service](Http::MessageResponse& response) {
        //TODO: review if && or const& should be used
        auto statusCode = response.getStatusCode();
        if (statusCode == 200) {
            LOG::Info("Successfuly get machine state from Python Db Agent");
            std::string responseStr = response.getBody();
            LOG::Debug(LOG::composeMessage("get response from db:", responseStr));
            service->sendResponse(200, std::move(responseStr));
        }
        else {
            auto responseStr = response.getBody();
            LOG::Error(LOG::composeMessage("Error while getting machine state from Python Db Agent", statusCode, responseStr));
            service->sendResponse(statusCode, std::move(responseStr));
        }
    };
    auto request = std::make_shared<Http::Request>(IoService::Get().getIoService(), "localhost", 10000, callback);
    request->get(machineName);
}

void DatabaseManager::setMachineState(const MachineState& machine) {
    auto callback = [](Http::MessageResponse& response) {
        if (response.getStatusCode() == 200) {
            LOG::Info("Successfuly save machine state to Python Db Agent");
        }
        else {
            LOG::Error("Error while setting machine state to Python Db Agent");
        }
    };
    auto request = std::make_shared<Http::Request>(IoService::Get().getIoService(), "localhost", 10000, callback);
    auto machineStr = JsonAdapter::machineStateToJson(machine).dump();
    LOG::Debug(LOG::composeMessage("Sending machine str to db:", machineStr));
    request->put(machine.mName, std::move(machineStr));
}

} // namespace ResourceMonitorServer