#include "Service.h"
#include "Log.h"
#include "DatabaseManager.h"
#include "JsonAdapter.h"

#include <map>

namespace ResourceMonitorServer {

Service::Service(TcpSocketPtr socket)
    : mSocket(socket)
    , mRequestBuf(4096)
    , mResponseStatusCode(200)
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
                    mResponseStatusCode = 413;
                    sendResponse();
                    return;
                }
                else {
                    LOG::Error(LOG::makeLogMessage("Error occured! Error code = ", ec.value(), ". Message: ", ec.message()));
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
    std::getline(requestStream, requestLine, '\r');
    requestStream.get(); // Remove symbol '\n' from the buffer.

    LOG::Debug(LOG::makeLogMessage("Request line: ", requestLine));

    std::string requestMethod;
    std::istringstream requestLineStream(requestLine);
    requestLineStream >> requestMethod;

    LOG::Debug(LOG::makeLogMessage("Request method: ", requestMethod));

    if (requestMethod != "GET") {
        mResponseStatusCode = 501;
        sendResponse();
        return;
    }

    requestLineStream >> mRequestedResource;

    LOG::Debug(LOG::makeLogMessage("Requested resource: ", mRequestedResource));

    std::string requestHttpVersion;
    requestLineStream >> requestHttpVersion;

    LOG::Debug(LOG::makeLogMessage("Request http version: ", requestHttpVersion));

    if (requestHttpVersion.compare("HTTP/1.1") != 0) {
        mResponseStatusCode = 505;
        sendResponse();

        return;
    }

    boost::asio::async_read_until(*mSocket.get(),
        mRequestBuf,
        "\r\n\r\n",
        [this] (const boost::system::error_code& ec, std::size_t bytes_transferred) {
            if (ec.value() != 0) {
                LOG::Error(LOG::makeLogMessage("Error occured! Error code = ", ec.value(), ". Message: ", ec.message()));

                if (ec == boost::asio::error::not_found) {
                    mResponseStatusCode = 413;
                    sendResponse();
                    return;
                }
                else {
                    finish();
                    return;
                }
            }
            processHeaders();
        }
    );
}

void Service::sendResponse() {
    LOG::Debug("Start sending response");

    mSocket->shutdown(boost::asio::ip::tcp::socket::shutdown_receive);

    std::string responseStatusLine = std::string("HTTP/1.1 ") + getStatusLine(mResponseStatusCode) + "\r\n";
    
    std::string responseHeadersString;
    for (const auto& [key, value] : mResponseHeaders) {
        responseHeadersString += key + ": " + value +"\r\n";
    }
    responseHeadersString += "\r\n";

    std::vector<boost::asio::const_buffer> responseBuffers;
    responseBuffers.reserve(3);
    responseBuffers.push_back(boost::asio::buffer(responseStatusLine));
    responseBuffers.push_back(boost::asio::buffer(responseHeadersString));

    if (mResponse.length() > 0) {
        responseBuffers.push_back(boost::asio::buffer(mResponse));
    }

    mResponse = responseStatusLine + responseHeadersString + mResponse;

    boost::asio::async_write(*mSocket.get(), boost::asio::buffer(mResponse),
        [this] (const boost::system::error_code& ec, std::size_t bytes_transferred) {
            if (ec.value() != 0) {
                LOG::Error(LOG::makeLogMessage("Error occured! Error code = ", ec.value(), ". Message: ", ec.message()));
            }

            mSocket->shutdown(boost::asio::ip::tcp::socket::shutdown_both);
            finish();
        }
    );
}

void Service::processHeaders() {
    LOG::Debug("Start headers processing");

    std::istream requestStream(&mRequestBuf);
    std::string headerName;
    std::string headerValue;

    while (!requestStream.eof()) {
        std::getline(requestStream, headerName, ':');
        if (!requestStream.eof()) {
            std::getline(requestStream, headerValue, '\r');
            requestStream.get();
            mRequestHeaders[headerName] = headerValue;
            LOG::Debug(LOG::makeLogMessage("Add header:", headerName, ":", headerValue));
        }
    }

    processRequest();
    sendResponse();

    return;
}

void Service::processRequest() {
    LOG::Debug("Request processing");

    const auto& machineState = DatabaseManager::Get().getMachineState("machine");
    const auto& jsonObject = JsonAdapter::machineStateToJson(machineState);
    mResponse = jsonObject.dump();

    LOG::Debug(LOG::makeLogMessage("Set response:", mResponse));

    mResponseHeaders["content-length"] = std::to_string(mResponse.length());
}

const std::string& Service::getStatusLine(HttpCode code) {
    static const std::map<HttpCode, std::string> httpStatusTable =
    {
        { 200, "200 OK" },
        { 404, "404 Not Found" },
        { 413, "413 Request Entity Too Large" },
        { 500, "500 Server Error" },
        { 501, "501 Not Implemented" },
        { 505, "505 HTTP Version Not Supported" }
    };
    return httpStatusTable.at(code);
}

void Service::finish() {
    LOG::Debug("Finish service");
    mSelfPtr.reset();
}

} // namespace ResourceMonitorServer