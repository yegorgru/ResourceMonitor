#include "DatabaseManager.h"

namespace ResourceMonitorServer {

MachineState DatabaseManager::getMachineState() const {
    return mMachineState;
}

void DatabaseManager::setMachineState(const MachineState& machine) {
    mMachineState = machine;
}

} // namespace ResourceMonitorServer