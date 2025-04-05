#pragma once
#include "IClient.h"
#include "IServer.h"

#include <memory>

namespace Http {

ClientPtr createNetworkClient();
ServerPtr createNetworkServer();

} // namespace Http