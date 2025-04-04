#pragma once
#include "IClient.h"

#include <memory>

namespace Http {

ClientPtr createNetworkClient();

} // namespace Http