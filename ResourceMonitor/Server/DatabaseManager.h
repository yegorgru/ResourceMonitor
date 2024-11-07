#pragma once

#include "MachineState.h"
#include "Singleton.h"

#include <string>

namespace ResourceMonitorServer {

class DatabaseManager : public Singleton<DatabaseManager>
{
    friend class Singleton<DatabaseManager>;
public:
    MachineState getMachineState() const;
    void setMachineState(const MachineState& machine);
private:
    DatabaseManager() = default;
private:
    MachineState mMachineState;
};

} // namespace ResourceMonitorServer