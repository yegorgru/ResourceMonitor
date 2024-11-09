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

std::optional<MachineState> jsonToMachineState(const std::string& jsonStr) {
    auto js = json::parse(jsonStr);
    if (!js.empty() || js["error"] != "") {
        return MachineState{
            .mName = js["name"],
            .mCpuUsage = js["cpu"]["usage %"],
            .mMemoryUsage = js["memory"]["usage %"],
            .mTotalMemory = js["memory"]["total GB"],
            .mMemoryUsed = js["memory"]["used GB"],
            .mDiskUsage = js["disk"]["usage %"],
            .mTotalDisk = js["disk"]["total GB"],
            .mDiskUsed = js["disk"]["used GB"]
        };
    }
    return std::nullopt;
}

} // namespace JsonAdapter