#pragma once

#include "MachineState.h"

#include <nlohmann/json.hpp>

namespace JsonAdapter
{

	using json = nlohmann::json;
	json machineStateToJson(MachineState machineState);
	MachineState jsonToMachineState(const std::string& jsonStr);

} // namespace JsonAdapter