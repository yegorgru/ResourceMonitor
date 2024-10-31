#include "HttpRequest.h"

namespace ResourceMonitorClient::Http {

Request::Request(IoService& ios, Id id) :
    mPort(DEFAULT_PORT),
    mId(id),
    mSock(ios),
    mResolver(ios),
    mWasCanceled(false),
    mIoService(ios)
{
}

Request::Id Request::getId() const {
    return mId;
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
    assert(mPort > 0);
    assert(mHost.length() > 0);
    assert(mUri.length() > 0);

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
}

void Request::connect(boost::asio::ip::tcp::resolver::iterator iterator) {
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
    std::string httpVersion;
    std::string strStatusCode;
    std::string statusMessage;

    std::istream responseStream(&mResponse.getResponseBuf());
    responseStream >> httpVersion;

    if (httpVersion != "HTTP/1.1") {
        finish(boost::system::error_code());
        return;
    }

    responseStream >> strStatusCode;

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
    if (ec.value() != 0) {
        std::cout << "Error occured! Error code = " << ec.value() << ". Message: " << ec.message();
    }
    else {
        std::cout << mResponse.getResponse().rdbuf() << std::endl;
    }
}

} // namespace ResourceMonitorClient::Http