#include "DatabaseManager.h"

namespace ResourceMonitorServer {

MachineState DatabaseManager::getMachineState(const std::string machine) {
    return MachineState{
        .mName = machine,
        .mCpuUsage = 1.2, 
        .mMemoryUsage = 3.4, 
        .mTotalMemory = 5.6, 
        .mMemoryUsed = 7.8, 
        .mDiskUsage = 9.0,
        .mTotalDisk = 11.12,
        .mDiskUsed = 13.14
    };
}

} // namespace ResourceMonitorServer