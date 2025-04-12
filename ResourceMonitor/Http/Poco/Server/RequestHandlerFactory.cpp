#include "RequestHandlerFactory.h"
#include "RequestHandler.h"
#include "Log.h"
#include "Print.h"
#include <set>

namespace Http::Poco {

RequestHandlerFactory::RequestHandlerFactory()
{
    Log::Debug("Creating Poco RequestHandlerFactory");
}

RequestHandlerFactory::RequestRawPtr RequestHandlerFactory::createRequestHandler(const ::Poco::Net::HTTPServerRequest& request)
{
    const std::string& uri = request.getURI();
    const std::string& method = request.getMethod();
    
    Log::Debug(Print::composeMessage("Received request:", method, uri, "from", request.clientAddress().toString()));
    
    return new RequestHandler();
}

} // namespace Http::Poco
