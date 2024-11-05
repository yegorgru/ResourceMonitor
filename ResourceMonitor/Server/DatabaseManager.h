#pragma once

#include "MachineState.h"
#include "Singleton.h"

#include <string>

namespace ResourceMonitorServer {

class DatabaseManager : public Singleton<DatabaseManager>
{
    friend class Singleton<DatabaseManager>;
public:
    MachineState getMachineState(const std::string machine);
private:
    DatabaseManager() = default;
};

} // namespace ResourceMonitorServer