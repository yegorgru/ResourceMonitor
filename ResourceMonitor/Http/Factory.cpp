#include "Factory.h"

#ifdef CLIENT_BOOST_ASIO
#include "Asio/Client/Client.h"
#else
#include "Beast/Client.h"
#endif

#ifdef SERVER_BOOST_ASIO
#include "Asio/Server/Server.h"
#else
#include "Beast/Client.h"
#endif

namespace Http {

ClientPtr createNetworkClient() {
    #ifdef CLIENT_BOOST_ASIO
    return std::make_unique<Asio::Client>();
    #else
    return std::make_unique<Beast::Client>();
    #endif
}

ServerPtr createNetworkServer() {
    #ifdef SERVER_BOOST_ASIO
    return std::make_unique<Asio::Server>();
    #else
    return std::make_unique<Beast::Server>();
    #endif
}

} // namespace Http