#include "DatabaseManager.h"
#include "HttpRequest.h"
#include "IoService.h"
#include "Log.h"

namespace ResourceMonitorServer {

void DatabaseManager::getMachineState(const std::string& machineName, ServicePtr service) const {
    auto callback = [service](Http::Message& response) {
        std::string responseStr = response.getBody();
        LOG::Debug(LOG::makeLogMessage("get response from db:", responseStr));
        service->sendResponse(std::move(responseStr));
    };
    auto request = std::make_shared<Http::Request>(IoService::Get().getIoService(), "localhost", 10000, callback);
    request->get(machineName);
}

void DatabaseManager::setMachineState(const MachineState& machine) {
    mMachineState = machine;
}

} // namespace ResourceMonitorServer