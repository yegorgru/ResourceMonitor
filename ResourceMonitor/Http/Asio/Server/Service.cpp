#include "Service.h"
#include "Log.h"
#include "BoostCommon/DatabaseManager.h"

#include <boost/algorithm/string/split.hpp>       
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include <map>
#include <set>
#include <deque>

namespace Http::Asio {

Service::Service(TcpSocketPtr socket)
    : mSocket(socket)
    , mRequestBuf(4096)
{
}

void Service::startHandling() {
    mSelfPtr = shared_from_this();
    //TODO: check if can use without get()
    boost::asio::async_read_until(*mSocket.get(), mRequestBuf, "\r\n",
        [this](const boost::system::error_code& ec, std::size_t bytes_transferred)
        {
            Log::Debug("Start handling new connection");
            if (ec.value() != 0) {
                if (ec == boost::asio::error::not_found) {
                    Log::Warning("Content Too Large");
                    sendResponse(StatusCode::RequestEntityTooLarge, "");
                    return;
                }
                else {
                    Log::Error(Print::composeMessage("Error occured! Error code = ", ec.value(), ". Message: ", ec.message()));
                    finish();
                    return;
                }
            }
            processRequestLine();
        }
    );
}

void Service::processRequestLine()
{
    Log::Debug("Start reading request line");

    std::string requestLine;
    std::istream requestStream(&mRequestBuf);
    std::getline(requestStream, requestLine, '\n');
    if (requestLine.back() != '\r') {
        Log::Error("Expected \\r\\n in request line");
        finish();
    }
    requestLine.pop_back();
    Log::Debug(Print::composeMessage("Request line: ", requestLine));

    std::istringstream requestLineStream(requestLine);
    std::string requestMethod;
    requestLineStream >> requestMethod;

    Log::Debug(Print::composeMessage("Request method: ", requestMethod));

    try
    {
        mRequest.setMethod(requestMethod);
    }
    catch (const std::runtime_error& ex)
    {
        Log::Error(Print::composeMessage(std::string(ex.what()) + "Method:", requestMethod));
        sendResponse(StatusCode::NotImplemented, "");
        return;
    }

    std::string endpoint;
    requestLineStream >> endpoint;
    Log::Debug(Print::composeMessage("Requested resource: ", endpoint));

    if (!isValidEndpoint(endpoint, requestMethod)) {
        Log::Warning(Print::composeMessage("Invalid endpoint:", endpoint));
        sendResponse(StatusCode::NotFound, "");
        return;
    }

    mRequest.setResource(endpoint);

    std::string requestHttpVersion;
    requestLineStream >> requestHttpVersion;

    Log::Debug(Print::composeMessage("Request http version: ", requestHttpVersion));

    if (requestHttpVersion != Message::STANDARD) {
        Log::Error(Print::composeMessage("Incorrect standard:", requestHttpVersion));
        sendResponse(StatusCode::HttpVersionNotSupported, "");
        return;
    }

    boost::asio::async_read_until(*mSocket.get(),
        mRequestBuf,
        "\r\n\r\n",
        [this] (const boost::system::error_code& ec, std::size_t bytes_transferred) {
            if (ec.value() != 0) {
                Log::Error(Print::composeMessage("Error occured! Error code = ", ec.value(), ". Message: ", ec.message()));

                if (ec == boost::asio::error::not_found) {
                    sendResponse(StatusCode::RequestEntityTooLarge, "");
                    return;
                }
                else if (ec == boost::asio::error::eof) {
                    Log::Debug("Eof, headers are absent");
                }
                else {
                    finish();
                    return;
                }
            }
            processHeadersAndContent();
        }
    );
}

void Service::processHeadersAndContent() {
    Log::Debug("Start headers processing");

    std::istream requestStream(&mRequestBuf);
    std::string line;

    while (std::getline(requestStream, line, '\n')) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        if (line.empty()) {
            break;
        }
        std::size_t colonPos = line.find(':');
        if (colonPos != std::string::npos) {
            std::string headerName = line.substr(0, colonPos);
            std::string headerValue = line.substr(colonPos + 1);
            headerValue.erase(0, headerValue.find_first_not_of(" \t"));
            mRequest.addHeader(headerName, headerValue);
            Log::Debug(Print::composeMessage("Add header:", headerName, ":", headerValue));
        }
    }

    mRequest.appendBody(std::string(std::istreambuf_iterator<char>(requestStream), std::istreambuf_iterator<char>()));
    Log::Debug(Print::composeMessage("Content:", mRequest.getBody()));

    auto sendToDbCallback = [this]() {
        using AsioIoService = boost::asio::io_service;
        using DatabaseManager = Boost::Common::DatabaseManager<AsioIoService, Request>;
        auto method = mRequest.getMethod();
        if (method == MessageRequest::Method::PUT) {
            Log::Debug("PUT request processing");
            auto callback = [this](const MessageResponse& response, const Id& id) {
                auto statusCode = response.getStatusCode();
                if (statusCode == StatusCode::Ok) {
                    Log::Debug(Print::composeMessage("Successfuly write info to database. Request:", id));
                    sendResponse(statusCode, "");
                }
                else {
                    if (statusCode == StatusCode::ClientClosedRequest) {
                        statusCode = StatusCode::ServerError;
                    }
                    Log::Error(Print::composeMessage("Error while writing info to database", static_cast<int>(statusCode), "Request:", id));
                    sendResponse(statusCode, response.getBody());
                }
            };
            DatabaseManager::Get().put(mRequest.getResource(), mRequest.getBody(), callback);
        }
        else if (method == MessageRequest::Method::GET) {
            Log::Debug("GET request processing");
            auto callback = [this](const MessageResponse& response, const Id&) {
                //TODO: review if && or const& should be used
                auto statusCode = response.getStatusCode();
                if (statusCode == StatusCode::Ok) {
                    Log::Debug("Successfuly get info from database");
                    sendResponse(StatusCode::Ok, std::move(response.getBody()));
                }
                else {
                    if (statusCode == StatusCode::ClientClosedRequest) {
                        statusCode = StatusCode::ServerError;
                    }
                    Log::Error(Print::composeMessage("Error while getting info from database", static_cast<int>(statusCode), response.getBody()));
                    sendResponse(statusCode, response.getBody());
                }
            };
            DatabaseManager::Get().get(mRequest.getResource(), callback);
        }
    };

    std::string contentLengthHeader = "Content-Length";
    if (mRequest.hasHeader(contentLengthHeader) && mRequest.getBody().length() < std::stoi(mRequest.getHeader(contentLengthHeader))) {
        boost::asio::async_read(*mSocket.get(), mRequestBuf,
            [this, sendToDbCallback](const boost::system::error_code& ec, std::size_t bytes_transferred)
            {
                std::istream requestStream(&mRequestBuf);
                mRequest.appendBody(std::string((std::istreambuf_iterator<char>(requestStream)), std::istreambuf_iterator<char>()));
                sendToDbCallback();
            }
        );
    }
    else {
        sendToDbCallback();
    }
}

void Service::sendResponse(StatusCode statusCode, const std::string& response) {
    Log::Debug("Start sending response");

    mSocket->shutdown(boost::asio::ip::tcp::socket::shutdown_receive);

    mResponse.setStatusCode(statusCode);
    mResponse.addHeader("Content-Length", std::to_string(response.length()));
    mResponse.addHeader("Access-Control-Allow-Origin", "*");
    mResponse.appendBody(response);

    boost::asio::async_write(*mSocket.get(), boost::asio::buffer(mResponse.createStringRepresentation()),
        [this](const boost::system::error_code& ec, std::size_t bytes_transferred) {
            if (ec.value() != 0) {
                Log::Error(Print::composeMessage("Error occured! Error code = ", ec.value(), ". Message: ", ec.message()));
            }

            mSocket->shutdown(boost::asio::ip::tcp::socket::shutdown_both);
            finish();
        }
    );
}

void Service::finish() {
    Log::Debug("Finish service");
    mSelfPtr.reset();
}

} // namespace Http::Asio