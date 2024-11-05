#include "HttpRequest.h"

#include "Log.h"
#include "JsonAdapter.h"

namespace ResourceMonitorClient::Http {

Request::Request(IoService& ios) 
    : mIsCompleted(false)
    , mPort(DEFAULT_PORT)
    , mSock(ios)
    , mResolver(ios)
    , mWasCanceled(false)
    , mIoService(ios)
{
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

bool Request::isCompleted() const {
    return mIsCompleted;
}

void Request::execute() {
    if (mPort == 0 || mHost == "" || mUri == "") {
        LOG::Throw(LOG::makeLogMessage("Incorrect request parameters. Port:", mPort, "host:", mHost, "uri:", mUri));
    }
    LOG::Debug(LOG::makeLogMessage("Start request executing. Port:", mPort, "host:", mHost, "uri:", mUri));

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
    mIsCompleted = true;
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

    mRequestBuf += "GET " + mUri + " HTTP/1.1\r\n";
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

    std::string header;
    std::string headerName;
    std::string headerValue;
    std::istream responseStream(&mResponse.getResponseBuf());

    while (true) {
        std::getline(responseStream, header, '\r');
        responseStream.get();

        if (header == "") {
            break;
        }

        size_t separatorPos = header.find(':');
        if (separatorPos != std::string::npos) {
            headerName = header.substr(0, separatorPos);
            headerValue = separatorPos < header.length() - 1 ? header.substr(separatorPos + 1) : "";
            mResponse.setHeader(headerName, headerValue);
            LOG::Debug(LOG::makeLogMessage("Get new header", headerName, ":", headerValue));
        }
    }

    if (mWasCanceled) {
        finish(boost::system::error_code(boost::asio::error::operation_aborted));
        return;
    }

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
        std::ostringstream oss;
        oss << mResponse.getResponse().rdbuf();
        std::string responseStr = oss.str();
        auto machineState = JsonAdapter::jsonToMachineState(responseStr);

        LOG::Info(LOG::makeLogMessage("Name:", machineState.mName));
        LOG::Info(LOG::makeLogMessage("CpuUsage:", machineState.mCpuUsage));
        LOG::Info(LOG::makeLogMessage("MemoryUsage:", machineState.mMemoryUsage));
        LOG::Info(LOG::makeLogMessage("TotalMemory:", machineState.mTotalMemory));
        LOG::Info(LOG::makeLogMessage("MemoryUsed:", machineState.mMemoryUsed));
        LOG::Info(LOG::makeLogMessage("DiskUsage:", machineState.mDiskUsage));
        LOG::Info(LOG::makeLogMessage("TotalDisk:", machineState.mTotalDisk));
        LOG::Info(LOG::makeLogMessage("DiskUsed:", machineState.mDiskUsed));

        auto message = LOG::makeLogMessage("Request processed successfully");
        LOG::Info(message);
        std::cout << message << std::endl;
    }
    mIsCompleted = true;
}

} // namespace ResourceMonitorClient::Http