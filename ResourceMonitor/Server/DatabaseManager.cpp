#include "DatabaseManager.h"
#include "HttpRequest.h"
#include "IoService.h"
#include "Log.h"
#include "JsonAdapter.h"

namespace ResourceMonitorServer {

void DatabaseManager::getMachineState(const std::string& machineName, ServicePtr service) const {
    auto callback = [service](Http::MessageResponse& response) {
        //TODO: review if && or const& should be used
        if (response.getStatusCode() == 200) {
            LOG::Info("Successfuly get machine state from Python Db Agent");
            std::string responseStr = response.getBody();
            LOG::Debug(LOG::makeLogMessage("get response from db:", responseStr));
            service->sendResponse(std::move(responseStr));
        }
        else {
            LOG::Error("Error while getting machine state from Python Db Agent");
            service->finish();
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
    LOG::Debug(LOG::makeLogMessage("Sending machine str to db:", machineStr));
    request->put(machine.mName, std::move(machineStr));
}

} // namespace ResourceMonitorServer