#include "Service.h"
#include "Log.h"
#include "DatabaseManager.h"

#include <boost/uuid/uuid_io.hpp>
#include <boost/algorithm/string/split.hpp>       
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include <map>
#include <set>
#include <deque>

namespace ResourceMonitorServer {

namespace {
    const std::set<std::string> validResources = {
        "basic_info",
        "cpu",
        "memory",
        "disks",
        "network"
    };
}

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
            LOG::Debug("Start handling new connection");
            if (ec.value() != 0) {
                if (ec == boost::asio::error::not_found) {
                    LOG::Warning("Content Too Large");
                    sendResponse(Http::StatusCode::RequestEntityTooLarge, "");
                    return;
                }
                else {
                    LOG::Error(PRINT::composeMessage("Error occured! Error code = ", ec.value(), ". Message: ", ec.message()));
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
    LOG::Debug("Start reading request line");

    std::string requestLine;
    std::istream requestStream(&mRequestBuf);
    std::getline(requestStream, requestLine, '\n');
    if (requestLine.back() != '\r') {
        LOG::Error("Expected \\r\\n in request line");
        finish();
    }
    requestLine.pop_back();
    LOG::Debug(PRINT::composeMessage("Request line: ", requestLine));

    std::istringstream requestLineStream(requestLine);
    std::string requestMethod;
    requestLineStream >> requestMethod;

    LOG::Debug(PRINT::composeMessage("Request method: ", requestMethod));

    try
    {
        mRequest.setMethod(requestMethod);
    }
    catch (const std::runtime_error& ex)
    {
        LOG::Error(PRINT::composeMessage(std::string(ex.what()) + "Method:", requestMethod));
        sendResponse(Http::StatusCode::NotImplemented, "");
        return;
    }

    std::string endpoint;
    requestLineStream >> endpoint;
    LOG::Debug(PRINT::composeMessage("Requested resource: ", endpoint));

    std::deque<std::string> splitedEndpoint;
    boost::algorithm::split(splitedEndpoint, endpoint, boost::is_any_of("/"), boost::token_compress_on);
    if (splitedEndpoint.front().empty()) {
        splitedEndpoint.pop_front();
    }
    if (splitedEndpoint.back().empty()) {
        splitedEndpoint.pop_back();
    }
    bool isValidEndpoint = false;
    if (mRequest.getMethod() == Http::MessageRequest::Method::GET && splitedEndpoint.size() == 3) {
        if (validResources.find(splitedEndpoint[0]) != validResources.end()) {
            int number;
            if (boost::conversion::try_lexical_convert<int>(splitedEndpoint[1], number) != false && number > 0) {
                boost::system::error_code ec;
                boost::asio::ip::address::from_string(splitedEndpoint[2], ec);
                isValidEndpoint = ec.value() == 0;
            }
        }
    }
    else if (mRequest.getMethod() == Http::MessageRequest::Method::PUT && splitedEndpoint.size() == 2) {
        if (validResources.find(splitedEndpoint[0]) != validResources.end()) {
            boost::system::error_code ec;
            boost::asio::ip::address::from_string(splitedEndpoint[1], ec);
            isValidEndpoint = ec.value() == 0;
        }
    }
    if (!isValidEndpoint) {
        sendResponse(Http::StatusCode::NotFound, "");
        return;
    }

    mRequest.setResource(endpoint);

    std::string requestHttpVersion;
    requestLineStream >> requestHttpVersion;

    LOG::Debug(PRINT::composeMessage("Request http version: ", requestHttpVersion));

    if (requestHttpVersion != Http::Message::STANDARD) {
        LOG::Error(PRINT::composeMessage("Incorrect standard:", requestHttpVersion));
        sendResponse(Http::StatusCode::HttpVersionNotSupported, "");
        return;
    }

    boost::asio::async_read_until(*mSocket.get(),
        mRequestBuf,
        "\r\n\r\n",
        [this] (const boost::system::error_code& ec, std::size_t bytes_transferred) {
            if (ec.value() != 0) {
                LOG::Error(PRINT::composeMessage("Error occured! Error code = ", ec.value(), ". Message: ", ec.message()));

                if (ec == boost::asio::error::not_found) {
                    sendResponse(Http::StatusCode::RequestEntityTooLarge, "");
                    return;
                }
                else if (ec == boost::asio::error::eof) {
                    LOG::Debug("Eof, headers are absent");
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
    LOG::Debug("Start headers processing");

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
            LOG::Debug(PRINT::composeMessage("Add header:", headerName, ":", headerValue));
        }
    }

    mRequest.appendBody(std::string(std::istreambuf_iterator<char>(requestStream), std::istreambuf_iterator<char>()));
    LOG::Debug(PRINT::composeMessage("Content:", mRequest.getBody()));

    auto sendToDbCallback = [this]() {
        auto method = mRequest.getMethod();
        if (method == Http::MessageRequest::Method::PUT) {
            LOG::Debug("PUT request processing");
            auto callback = [this](const Http::MessageResponse& response, const Http::Request::Id& id) {
                auto statusCode = response.getStatusCode();
                if (statusCode == Http::StatusCode::Ok) {
                    LOG::Info(PRINT::composeMessage("Successfuly write info to database. Request:", boost::uuids::to_string(id)));
                    sendResponse(statusCode, "");
                }
                else {
                    if (statusCode == Http::StatusCode::ClientClosedRequest) {
                        statusCode = Http::StatusCode::ServerError;
                    }
                    LOG::Error(PRINT::composeMessage("Error while writing info to database", static_cast<int>(statusCode), "Request:", boost::uuids::to_string(id)));
                    sendResponse(statusCode, response.getBody());
                }
            };
            DatabaseManager::Get().put(mRequest.getResource(), mRequest.getBody(), callback);
        }
        else if (method == Http::MessageRequest::Method::GET) {
            LOG::Debug("GET request processing");
            auto callback = [this](const Http::MessageResponse& response, const Http::Request::Id&) {
                //TODO: review if && or const& should be used
                auto statusCode = response.getStatusCode();
                if (statusCode == Http::StatusCode::Ok) {
                    LOG::Info("Successfuly get info from database");
                    std::string responseStr = response.getBody();
                    LOG::Debug(PRINT::composeMessage("get response from db:", responseStr));
                    sendResponse(Http::StatusCode::Ok, std::move(responseStr));
                }
                else {
                    if (statusCode == Http::StatusCode::ClientClosedRequest) {
                        statusCode = Http::StatusCode::ServerError;
                    }
                    auto responseStr = response.getBody();
                    LOG::Error(PRINT::composeMessage("Error while getting info from database", static_cast<int>(statusCode), responseStr));
                    sendResponse(statusCode, std::move(responseStr));
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

void Service::sendResponse(Http::StatusCode statusCode, const std::string& response) {
    LOG::Debug("Start sending response");

    mSocket->shutdown(boost::asio::ip::tcp::socket::shutdown_receive);

    mResponse.setStatusCode(statusCode);
    mResponse.addHeader("Content-Length", std::to_string(response.length()));
    mResponse.addHeader("Access-Control-Allow-Origin", "*");
    mResponse.appendBody(response);

    boost::asio::async_write(*mSocket.get(), boost::asio::buffer(mResponse.createStringRepresentation()),
        [this](const boost::system::error_code& ec, std::size_t bytes_transferred) {
            if (ec.value() != 0) {
                LOG::Error(PRINT::composeMessage("Error occured! Error code = ", ec.value(), ". Message: ", ec.message()));
            }

            mSocket->shutdown(boost::asio::ip::tcp::socket::shutdown_both);
            finish();
        }
    );
}

void Service::finish() {
    LOG::Debug("Finish service");
    mSelfPtr.reset();
}

} // namespace ResourceMonitorServer