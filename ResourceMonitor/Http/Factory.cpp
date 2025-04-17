#include "Factory.h"

#if defined(USE_ASIO)
#include "Asio/Client/Client.h"
#include "BoostCommon/Server.h"
#include "Asio/Server/Service.h"
#elif defined(USE_BEAST)
#include "Beast/Client/Client.h"
#include "BoostCommon/Server.h"
#include "Beast/Server/Service.h"
#elif defined(USE_POCO)
#include "Poco/Client/Client.h"
#include "Poco/Server/Server.h"
#else 
#error "No http implementation selected."
#endif

namespace Http {

ClientPtr createNetworkClient() {
#if defined(USE_ASIO)
    return std::make_unique<Asio::Client>();
#elif defined(USE_BEAST)
    return std::make_unique<Beast::Client>();
#elif defined(USE_POCO)
    return std::make_unique<Poco::Client>();
#else 
    return nullptr;
#endif
}

ServerPtr createNetworkServer() {
#if defined(USE_ASIO)
    using ServerType = Boost::Common::Server<boost::asio::io_service, Asio::Service>;
    return std::make_unique<ServerType>();
#elif defined(USE_BEAST)
    using ServerType = Boost::Common::Server<boost::asio::io_context, Beast::Service>;
    return std::make_unique<ServerType>();
#elif defined(USE_POCO)
    return std::make_unique<Poco::Server>();
#else 
    return nullptr;
#endif
}

} // namespace Http