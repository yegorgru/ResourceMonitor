#include "DatabaseManager.h"
#include "HttpRequest.h"
#include "IoService.h"
#include "Log.h"

namespace ResourceMonitorServer {

void DatabaseManager::getMachineState(const std::string& machineName, ServicePtr service) const {
    auto callback = [service](Http::Response& response) {
        std::string responseStr = response.getBody();
        LOG::Debug(LOG::makeLogMessage("get response from db:", responseStr));
        service->sendResponse(std::move(responseStr));
    };
    auto request = std::make_shared<Http::Request>(IoService::Get().getIoService(), callback);
    request->setMethod(Http::Method::GET);
    request->setHost("localhost");
    request->setUri("/" + machineName);
    request->setPort(10000);
    request->execute();
}

void DatabaseManager::setMachineState(const MachineState& machine) {
    mMachineState = machine;
}

} // namespace ResourceMonitorServer