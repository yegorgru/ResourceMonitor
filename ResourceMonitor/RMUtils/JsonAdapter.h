#pragma once

#include "MachineState.h"

#include <nlohmann/json.hpp>
#include <optional>

namespace JsonAdapter
{

	using json = nlohmann::json;
	json machineStateToJson(MachineState machineState);
	std::optional<MachineState> jsonToMachineState(const std::string& jsonStr);

} // namespace JsonAdapter