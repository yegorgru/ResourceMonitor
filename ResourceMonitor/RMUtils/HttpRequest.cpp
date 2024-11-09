#include "pch.h"
#include "HttpRequest.h"

#include "Log.h"
#include "JsonAdapter.h"

namespace Http {

Request::Request(IoService& ios, Callback callback)
    : mMethod(Method::GET)
    , mPort(DEFAULT_PORT)
    , mCallback(callback)
    , mSock(ios)
    , mResolver(ios)
    , mWasCanceled(false)
    , mIoService(ios)
{
}

void Request::setMethod(Method method) {
    mMethod = method;
}

void Request::setHost(const std::string& host) {
    mHost = host;
}

std::string Request::getHost() const {
    return mHost;
}

void Request::setPort(Port port) {
    mPort = port;
}

Request::Port Request::getPort() const {
    return mPort;
}

void Request::setUri(const std::string& uri) {
    mUri = uri;
}

const std::string& Request::getUri() const {
    return mUri;
}

void Request::execute() {
    if (mPort == 0 || mHost == "" || mUri == "") {
        LOG::Throw(LOG::makeLogMessage("Incorrect request parameters. Port:", mPort, "host:", mHost, "uri:", mUri));
    }
    LOG::Debug(LOG::makeLogMessage("Start request executing. Port:", mPort, "host:", mHost, "uri:", mUri));

    mSelfPtr = shared_from_this();

    boost::asio::ip::tcp::resolver::query resolverQuery(
        mHost, std::to_string(mPort), boost::asio::ip::tcp::resolver::query::numeric_service
    );

    if (mWasCanceled) {
        finish(boost::system::error_code(boost::asio::error::operation_aborted));
        return;
    }

    mResolver.async_resolve(resolverQuery,
        [this](const boost::system::error_code& ec, boost::asio::ip::tcp::resolver::iterator iterator) {
            if (ec.value() != 0) {
                finish(ec);
                return;
            }
            connect(iterator);
        }
    );
}

void Request::cancel() {
    mWasCanceled = true;
    mResolver.cancel();
    if (mSock.is_open()) {
        mSock.cancel();
    }
    mSelfPtr.reset();
}

void Request::connect(boost::asio::ip::tcp::resolver::iterator iterator) {
    LOG::Debug("Start connecting to server");
    if (mWasCanceled) {
        finish(boost::system::error_code(boost::asio::error::operation_aborted));
        return;
    }
    boost::asio::async_connect(mSock, iterator,
        [this](const boost::system::error_code& ec, boost::asio::ip::tcp::resolver::iterator iterator)
        {
            if (ec.value() != 0) {
                finish(ec);
                return;
            }
            sendRequest();
        }
    );
}

void Request::sendRequest() {
    LOG::Debug("Start request creation");

    mRequestBuf += methodToStr(mMethod) + " " + mUri + " HTTP/1.1\r\n";
    mRequestBuf += "Host: " + mHost + "\r\n";
    mRequestBuf += "\r\n";

    if (mWasCanceled) {
        finish(boost::system::error_code(boost::asio::error::operation_aborted));
        return;
    }

    boost::asio::async_write(mSock, boost::asio::buffer(mRequestBuf),
        [this](const boost::system::error_code& ec, std::size_t bytes_transferred)
        {
            if (ec.value() != 0) {
                finish(ec);
                return;
            }
            LOG::Debug("Request sent");
            readResponse();
        }
    );
}

void Request::readResponse()
{
    mSock.shutdown(boost::asio::ip::tcp::socket::shutdown_send);

    if (mWasCanceled) {
        finish(boost::system::error_code(boost::asio::error::operation_aborted));
        return;
    }

    LOG::Debug("Start response reading");

    boost::asio::async_read_until(mSock, mResponse.getResponseBuf(), "\r\n",
        [this](const boost::system::error_code& ec, std::size_t bytes_transferred)
        {
            if (ec.value() != 0) {
                finish(ec);
                return;
            }
            readStatusLine();
        }
    );
}

void Request::readStatusLine()
{
    LOG::Debug("Start response status line reading");

    std::string httpVersion;
    std::string strStatusCode;
    std::string statusMessage;

    std::istream responseStream(&mResponse.getResponseBuf());
    responseStream >> httpVersion;

    LOG::Debug(LOG::makeLogMessage("Http version:", httpVersion));

    if (httpVersion != "HTTP/1.1") {
        finish(boost::system::error_code());
        return;
    }

    responseStream >> strStatusCode;

    LOG::Debug(LOG::makeLogMessage("Status code:", strStatusCode));

    unsigned int statusCode = 200;

    try {
        statusCode = std::stoul(strStatusCode);
    }
    catch (std::logic_error&) {
        finish(boost::system::error_code());
        return;
    }

    std::getline(responseStream, statusMessage, '\r');
    // Remove symbol '\n' from the buffer.
    responseStream.get();

    LOG::Debug(LOG::makeLogMessage("Status message:", statusMessage));

    mResponse.setStatusCode(statusCode);
    mResponse.setStatusMessage(statusMessage);

    if (mWasCanceled) {
        finish(boost::system::error_code(boost::asio::error::operation_aborted));
        return;
    }

    boost::asio::async_read_until(mSock, mResponse.getResponseBuf(),
        "\r\n\r\n",
        [this](const boost::system::error_code& ec, size_t bytesTransferred)
        {
            if (ec.value() != 0) {
                finish(ec);
                return;
            }
            readHeaders();
        }
    );
}

void Request::readHeaders()
{
    LOG::Debug("Start response headers reading");

    std::istream requestStream(&mResponse.getResponseBuf());
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
            mResponse.setHeader(headerName, headerValue);
            LOG::Debug(LOG::makeLogMessage("Add header:", headerName, ":", headerValue));
        }
    }

    mResponse.setBody(std::string((std::istreambuf_iterator<char>(requestStream)), std::istreambuf_iterator<char>()));

    boost::asio::async_read(mSock, mResponse.getResponseBuf(),
        [this](const boost::system::error_code& ec, std::size_t bytes_transferred)
        {
            if (ec == boost::asio::error::eof) {
                finish(boost::system::error_code());
            }
            else {
                finish(ec);
            }
        }
    );
}

void Request::finish(const boost::system::error_code& ec) {
    LOG::Debug("Finish request");
    if (ec.value() != 0) {
        auto message = LOG::makeLogMessage("Error occured! Error code =", ec.value(), ". Message:", ec.message());
        LOG::Error(message);
        std::cout << message << std::endl;
    }
    else {
        mCallback(mResponse);
    }
    mSelfPtr.reset();
}

} // namespace ResourceMonitorClient::Http