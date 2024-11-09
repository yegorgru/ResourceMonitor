#pragma once

#include "MachineState.h"
#include "Singleton.h"
#include "Service.h"

#include <string>

namespace ResourceMonitorServer {

class DatabaseManager : public Singleton<DatabaseManager>
{
    friend class Singleton<DatabaseManager>;
public:
    void getMachineState(const std::string& machineName, ServicePtr service) const;
    void setMachineState(const MachineState& machine);
private:
    DatabaseManager() = default;
};

} // namespace ResourceMonitorServer