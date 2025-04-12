#include "RequestHandler.h"
#include "DatabaseManager.h"
#include "Log.h"
#include "Print.h"

namespace Http::Poco {

RequestHandler::RequestHandler()
{
    Log::Debug("Creating Poco RequestHandler");
}

void RequestHandler::handleRequest(Request& request, Response& response)
{
    auto resource = request.getURI();
    const std::string& method = request.getMethod();
    
    Log::Debug(Print::composeMessage("Handling request:", method, resource));
    
    std::string body;
    if (request.getContentLength() > 0) {
        std::istream& bodyStream = request.stream();
        body = std::string(std::istreambuf_iterator<char>(bodyStream), {});
        Log::Debug(Print::composeMessage("Request body:", body));
    }
    if (!resource.empty() && resource[0] == '/') {
        resource = resource.substr(1);
    }
    
    try {
        if (!Http::isValidEndpoint(resource, method)) {
            Log::Warning(Print::composeMessage("Invalid endpoint:", resource));
            sendResponse(response, StatusCode::NotFound, "Invalid endpoint");
            return;
        }
        
        if (method == "GET") {
            processGetRequest(resource, body, response);
        }
        else if (method == "PUT") {
            processPutRequest(resource, body, response);
        }
        else {
            Log::Warning(Print::composeMessage("Unsupported method:", method));
            sendResponse(response, StatusCode::NotImplemented);
        }
    }
    catch (const std::exception& e) {
        Log::Error(Print::composeMessage("Error handling request:", e.what()));
        sendResponse(response, StatusCode::ServerError, "Internal Server Error");
    }
}

void RequestHandler::processGetRequest(const std::string& resource, const std::string& body, Response& response)
{
    Log::Debug("Processing GET request");
    
    auto callback = [this, &response](const Session::ResponseData& respData, const Id& id) {
        auto statusCode = respData.mStatusCode;
        if (statusCode == StatusCode::Ok) {
            Log::Debug("Successfully got info from database");
            sendResponse(response, statusCode, respData.mBody);
        }
        else {
            if (statusCode == StatusCode::ClientClosedRequest) {
                statusCode = StatusCode::ServerError;
            }
            Log::Error(Print::composeMessage("Error while getting info from database", static_cast<int>(statusCode), respData.mBody));
            sendResponse(response, statusCode, respData.mBody);
        }
    };
    
    DatabaseManager::Get().get(resource, callback);
}

void RequestHandler::processPutRequest(const std::string& resource, const std::string& body, Response& response)
{
    Log::Debug("Processing PUT request");
    
    auto callback = [this, &response](const Session::ResponseData& respData, const Id& id) {
        auto statusCode = respData.mStatusCode;
        if (statusCode == StatusCode::Ok) {
            Log::Debug(Print::composeMessage("Successfully wrote info to database. Request:", id));
            sendResponse(response, statusCode, "");
        }
        else {
            if (statusCode == StatusCode::ClientClosedRequest) {
                statusCode = StatusCode::ServerError;
            }
            Log::Error(Print::composeMessage("Error while writing info to database", static_cast<int>(statusCode), "Request:", id));
            sendResponse(response, statusCode, respData.mBody);
        }
    };
    
    DatabaseManager::Get().put(resource, body, callback);
}

void RequestHandler::sendResponse(Response& response, StatusCode status, const std::string& body)
{
    Log::Debug("Sending response");
    
    response.setStatus(static_cast<::Poco::Net::HTTPResponse::HTTPStatus>(status));
    response.setContentLength(body.length());
    response.set("Access-Control-Allow-Origin", "*");
    
    std::ostream& ostr = response.send();
    ostr << body;
    ostr.flush();
    
    Log::Debug(Print::composeMessage("Response sent with status:", static_cast<int>(status)));
}

} // namespace Http::Poco
