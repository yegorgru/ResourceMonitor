#include "Service.h"

#include <iostream>
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
            if (ec.value() != 0) {
                std::cout << "Error occured! Error code = " << ec.value() << ". Message: " << ec.message();

                if (ec == boost::asio::error::not_found) {
                    mResponseStatusCode = 413;
                    sendResponse();
                    return;
                }
                else {
                    onFinish();
                    return;
                }
            }
            processRequestLine();
        }
    );
}

void Service::processRequestLine()
{
    std::string requestLine;
    std::istream requestStream(&mRequestBuf);
    std::getline(requestStream, requestLine, '\r');

    requestStream.get(); // Remove symbol '\n' from the buffer.

    std::string requestMethod;
    std::istringstream requestLineStream(requestLine);
    requestLineStream >> requestMethod;

    if (requestMethod.compare("GET") != 0) {
        mResponseStatusCode = 501;
        sendResponse();
        return;
    }

    requestLineStream >> mRequestedResource;

    std::string request_http_version;
    requestLineStream >> request_http_version;

    if (request_http_version.compare("HTTP/1.1") != 0) {
        mResponseStatusCode = 505;
        sendResponse();

        return;
    }

    boost::asio::async_read_until(*mSocket.get(),
        mRequestBuf,
        "\r\n\r\n",
        [this] (const boost::system::error_code& ec, std::size_t bytes_transferred) {
            if (ec.value() != 0) {
                std::cout << "Error occured! Error code = " << ec.value() << ". Message: " << ec.message();

                if (ec == boost::asio::error::not_found) {
                    mResponseStatusCode = 413;
                    sendResponse();
                    return;
                }
                else {
                    onFinish();
                    return;
                }
            }
            processHeaders();
        }
    );
}

void Service::sendResponse() {
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
                std::cout << "Error occured! Error code = "
                    << ec.value()
                    << ". Message: " << ec.message();
            }

            mSocket->shutdown(boost::asio::ip::tcp::socket::shutdown_both);
            onFinish();
        }
    );
}

void Service::processHeaders() {
    std::istream requestStream(&mRequestBuf);
    std::string headerName, headerValue;

    while (!requestStream.eof()) {
        std::getline(requestStream, headerName, ':');
        if (!requestStream.eof()) {
            std::getline(requestStream, headerValue, '\r');
            requestStream.get();
            mRequestHeaders[headerName] = headerValue;
        }
    }

    processRequest();
    sendResponse();

    return;
}

void Service::processRequest() {
    mResponse = "Content";
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

void Service::onFinish() {
    mSelfPtr.reset();
}

} // namespace ResourceMonitorServer