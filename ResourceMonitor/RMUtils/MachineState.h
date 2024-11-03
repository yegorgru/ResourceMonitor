#pragma once

#include <string>

struct MachineState {
    std::string mName;
    float mCpuUsage;
    float mMemoryUsage;
    float mTotalMemory;
    float mMemoryUsed;
    float mDiskUsage;
    float mTotalDisk;
    float mDiskUsed;
};