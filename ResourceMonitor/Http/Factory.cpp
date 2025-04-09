#include "Factory.h"

#ifdef CLIENT_BOOST_ASIO
#include "Asio/Client/Client.h"
#else
#include "Beast/Client/Client.h"
#endif

#ifdef SERVER_BOOST_ASIO
#include "Asio/Server/Service.h"
#else
#include "Beast/Server/Service.h"
#endif
#include "Server.h"

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
    using ServerType = Commons::Server<boost::asio::io_service, Asio::Service>;
    return std::make_unique<ServerType>();
    #else
    using ServerType = Commons::Server<boost::asio::io_context, Beast::Service>;
    return std::make_unique<ServerType>();
    #endif
}

} // namespace Http