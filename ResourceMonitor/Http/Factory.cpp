#include "Factory.h"

#if defined(CLIENT_BOOST_ASIO) + defined(CLIENT_BOOST_BEAST) + defined(CLIENT_POCO) > 1
#error "Only one client macro can be defined: CLIENT_BOOST_ASIO, CLIENT_BOOST_BEAST, or CLIENT_POCO."
#endif

#if defined(SERVER_BOOST_ASIO) + defined(SERVER_BOOST_BEAST) + defined(SERVER_POCO) > 1
#error "Only one server macro can be defined: SERVER_BOOST_ASIO, SERVER_BOOST_BEAST, or SERVER_POCO."
#endif

#if defined(CLIENT_BOOST_ASIO)
#include "Asio/Client/Client.h"
#elif defined(CLIENT_BOOST_BEAST)
#include "Beast/Client/Client.h"
#elif defined(CLIENT_POCO)
#include "Poco/Client/Client.h"
#else 
#error "No client macro defined. Define one of CLIENT_BOOST_ASIO, CLIENT_BOOST_BEAST, or CLIENT_POCO."
#endif

#if defined(SERVER_BOOST_ASIO)
#include "Asio/Server/Service.h"
#elif defined(SERVER_BOOST_BEAST)
#include "Beast/Server/Service.h"
#elif defined(SERVER_POCO)
#include "Poco/Server/Service.h"
#else 
#error "No server macro defined. Define one of SERVER_BOOST_ASIO, SERVER_BOOST_BEAST, or SERVER_POCO."
#endif

#include "Server.h"

namespace Http {

ClientPtr createNetworkClient() {
#if defined(CLIENT_BOOST_ASIO)
    return std::make_unique<Asio::Client>();
#elif defined(CLIENT_BOOST_BEAST)
    return std::make_unique<Beast::Client>();
#elif defined(CLIENT_POCO)
    return std::make_unique<Poco::Client>();
#else 
    return nullptr;
#endif
}

ServerPtr createNetworkServer() {
#if defined(SERVER_BOOST_ASIO)
    using ServerType = Commons::Server<boost::asio::io_service, Asio::Service>;
    return std::make_unique<ServerType>();
#elif defined(SERVER_BOOST_BEAST)
    using ServerType = Commons::Server<boost::asio::io_context, Beast::Service>;
    return std::make_unique<ServerType>();
#elif defined(SERVER_POCO)
    #error "Poco server not implemented yet."
#else 
    return nullptr;
#endif
}

} // namespace Http