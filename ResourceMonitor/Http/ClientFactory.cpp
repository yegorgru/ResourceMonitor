#include "ClientFactory.h"

#ifdef USE_BOOST_ASIO
#include "Asio/Client.h"
#else
#include "Beast/Client.h"
#endif

namespace Http {

ClientPtr createNetworkClient() {
    #ifdef USE_BOOST_ASIO
    return std::make_unique<Asio::Client>();
    #else
    return std::make_unique<Beast::Client>();
    #endif
}

} // namespace Http