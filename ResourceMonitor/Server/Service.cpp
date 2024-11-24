#include "Service.h"
#include "Log.h"
#include "DatabaseManager.h"

#include <map>

namespace ResourceMonitorServer {

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
                    sendResponse(413, "");
                    return;
                }
                else {
                    LOG::Error(LOG::composeMessage("Error occured! Error code = ", ec.value(), ". Message: ", ec.message()));
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
    LOG::Debug(LOG::composeMessage("Request line: ", requestLine));

    std::istringstream requestLineStream(requestLine);
    std::string requestMethod;
    requestLineStream >> requestMethod;

    LOG::Debug(LOG::composeMessage("Request method: ", requestMethod));

    try
    {
        mRequest.setMethod(requestMethod);
    }
    catch (const std::runtime_error& ex)
    {
        LOG::Error(LOG::composeMessage("Method is not implemented:", requestMethod));
        sendResponse(501, "");
        return;
    }

    std::string machineName;
    requestLineStream >> machineName;
    LOG::Debug(LOG::composeMessage("Requested resource: ", machineName));
    mRequest.setResource(machineName);

    std::string requestHttpVersion;
    requestLineStream >> requestHttpVersion;

    LOG::Debug(LOG::composeMessage("Request http version: ", requestHttpVersion));

    if (requestHttpVersion != Http::Message::STANDARD) {
        LOG::Error(LOG::composeMessage("Incorrect standard:", requestHttpVersion));
        sendResponse(505, "");
        return;
    }

    boost::asio::async_read_until(*mSocket.get(),
        mRequestBuf,
        "\r\n\r\n",
        [this] (const boost::system::error_code& ec, std::size_t bytes_transferred) {
            if (ec.value() != 0) {
                LOG::Error(LOG::composeMessage("Error occured! Error code = ", ec.value(), ". Message: ", ec.message()));

                if (ec == boost::asio::error::not_found) {
                    sendResponse(413, "");
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
            LOG::Debug(LOG::composeMessage("Add header:", headerName, ":", headerValue));
        }
    }

    mRequest.setBody(std::string(std::istreambuf_iterator<char>(requestStream), std::istreambuf_iterator<char>()));
    LOG::Debug(LOG::composeMessage("Content:", mRequest.getBody()));

    auto method = mRequest.getMethod();
    if (method == Http::MessageRequest::Method::PUT) {
        LOG::Debug("PUT request processing");
        auto callback = [](Http::MessageResponse& response) {
            if (response.getStatusCode() == 200) {
                LOG::Info("Successfuly write info to database");
            }
            else {
                LOG::Error("Error while writing info to database");
            }
        };
        DatabaseManager::Get().put(mRequest.getResource(), mRequest.getBody(), callback);
        LOG::Debug(LOG::composeMessage("Put info to database", mRequest.getResource(), mRequest.getBody()));
        sendResponse(200, "");
    }
    else if (method == Http::MessageRequest::Method::GET) {
        LOG::Debug("GET request processing");
        auto callback = [this](Http::MessageResponse& response) {
            //TODO: review if && or const& should be used
            auto statusCode = response.getStatusCode();
            if (statusCode == 200) {
                LOG::Info("Successfuly get info from database");
                std::string responseStr = response.getBody();
                LOG::Debug(LOG::composeMessage("get response from db:", responseStr));
                sendResponse(200, std::move(responseStr));
            }
            else {
                auto responseStr = response.getBody();
                LOG::Error(LOG::composeMessage("Error while getting info from database", statusCode, responseStr));
                sendResponse(statusCode, std::move(responseStr));
            }
        };
        DatabaseManager::Get().get(mRequest.getResource(), callback);
    }
}

void Service::sendResponse(int statusCode, const std::string& response) {
    LOG::Debug("Start sending response");

    mSocket->shutdown(boost::asio::ip::tcp::socket::shutdown_receive);

    mResponse.setStatusCode(statusCode);
    mResponse.addHeader("Content-Length", std::to_string(response.length()));
    mResponse.addHeader("Access-Control-Allow-Origin", "*");
    mResponse.setBody(response);

    boost::asio::async_write(*mSocket.get(), boost::asio::buffer(mResponse.createStringRepresentation()),
        [this](const boost::system::error_code& ec, std::size_t bytes_transferred) {
            if (ec.value() != 0) {
                LOG::Error(LOG::composeMessage("Error occured! Error code = ", ec.value(), ". Message: ", ec.message()));
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