#pragma once

#include "MachineState.h"

#include <string>

namespace ResourceMonitorServer {

class DatabaseManager
{
public:
    MachineState getMachineState(const std::string machine);
};
} // namespace ResourceMonitorServer