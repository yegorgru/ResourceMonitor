#pragma once

#include "MachineState.h"

#include <nlohmann/json.hpp>

namespace JsonAdapter
{

	using json = nlohmann::json;
	json machineStateToJson(MachineState machineState);

} // namespace JsonAdapter