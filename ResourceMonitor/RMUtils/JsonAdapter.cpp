#include "pch.h"
#include "JsonAdapter.h"

namespace JsonAdapter{

json machineStateToJson(MachineState machineState) {
    return {
        {"name", machineState.mName},
        {"cpu", {
            {"usage %", machineState.mCpuUsage}
        }},
        {"memory", {
            {"usage %", machineState.mMemoryUsage},
            {"total GB", machineState.mTotalMemory},
            {"used GB", machineState.mMemoryUsed}
        }},
        {"disk", {
            {"usage %", machineState.mDiskUsed},
            {"total GB", machineState.mTotalDisk},
            {"used GB", machineState.mDiskUsed}
        }}
    };
}

} // namespace JsonAdapter