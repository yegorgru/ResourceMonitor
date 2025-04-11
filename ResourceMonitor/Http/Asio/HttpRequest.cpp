#include "HttpRequest.h"
#include "Log.h"
#include "Utils.h"

namespace Http::Asio {

Request::Request(IoService& ios, const std::string& host, Port port, Callback callback)
    : mId(generateId())
    , mHost(host)
    , mPort(port)
    , mCallback(callback)
    , mSock(ios)
    , mResolver(ios)
    , mIsCanceled(false)
    , mIoService(ios)
{
}

Request::~Request() {
    Log::Trace("Request destructor");
}

void Request::get(const std::string& resource) {
    mRequestMessage.setMethod("GET");
    mRequestMessage.setResource(resource);
    execute();
}

void Request::put(const std::string& resource, const std::string& body) {
    mRequestMessage.setMethod("PUT");
    mRequestMessage.setResource(resource);
    mRequestMessage.appendBody(body);
    execute();
}

void Request::addHeader(const std::string& name, const std::string& value) {
    mRequestMessage.addHeader(name, value);
}

void Request::execute() {
    const auto& res = mRequestMessage.getResource();
    if (!isValidPort(mPort) || mHost == "" || res == "") {
        Log::Throw(Print::composeMessage("Incorrect request parameters. Port:", mPort, "host:", mHost, "resource:", res));
    }
    Log::Debug(Print::composeMessage("Start request executing. Port:", mPort, "host:", mHost, "resource:", res));

    mSelfPtr = shared_from_this();

    boost::asio::ip::tcp::resolver::query resolverQuery(
        mHost, std::to_string(mPort), boost::asio::ip::tcp::resolver::query::numeric_service
    );

    if (mIsCanceled) {
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
    if (isCompleted()) {
        return;
    }
    Log::Debug(Print::composeMessage("Cancelling request. Id:", mId));
    mResponseMessage.setStatusCode(StatusCode::ClientClosedRequest);
    mCallback(mResponseMessage, mId);
    mIsCanceled = true;
    mResolver.cancel();
    if (mSock.is_open()) {
        mSock.cancel();
    }
    mSelfPtr.reset();
}

bool Request::isCompleted() {
    return mSelfPtr == nullptr;
}

const Id& Request::getId() const {
    return mId;
}

void Request::connect(boost::asio::ip::tcp::resolver::iterator iterator) {
    Log::Debug("Start connecting to server");
    if (mIsCanceled) {
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
    Log::Debug("Start request creation");

    mRequestMessage.addHeader("Host", mHost);

    if (mIsCanceled) {
        finish(boost::system::error_code(boost::asio::error::operation_aborted));
        return;
    }

    boost::asio::async_write(mSock, boost::asio::buffer(mRequestMessage.createStringRepresentation()),
        [this](const boost::system::error_code& ec, std::size_t bytes_transferred)
        {
            if (ec.value() != 0) {
                finish(ec);
                return;
            }
            Log::Debug("Request sent");
            readResponse();
        }
    );
}

void Request::readResponse()
{
    mSock.shutdown(boost::asio::ip::tcp::socket::shutdown_send);

    if (mIsCanceled) {
        finish(boost::system::error_code(boost::asio::error::operation_aborted));
        return;
    }

    Log::Debug("Start waiting for response");

    boost::asio::async_read_until(mSock, mResponseBuf, "\r\n",
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
    Log::Debug("Start response status line reading");

    std::string httpVersion;
    std::string strStatusCode;
    std::string statusMessage;

    std::istream responseStream(&mResponseBuf);
    responseStream >> httpVersion;

    Log::Debug(Print::composeMessage("Http version:", httpVersion));

    if (httpVersion != "HTTP/1.1") {
        Log::Error(Print::composeMessage("Unsupported HTTP version:", httpVersion));
        finish(boost::system::errc::make_error_code(boost::system::errc::protocol_error));
        return;
    }

    responseStream >> strStatusCode;

    Log::Debug(Print::composeMessage("Status code:", strStatusCode));

    StatusCode statusCode = StatusCode::Ok;

    try {
        statusCode = intToStatusCode(std::stoi(strStatusCode));
    }
    catch (std::logic_error& ex) {
        Log::Error(ex.what());
        finish(boost::system::errc::make_error_code(boost::system::errc::invalid_argument));
        return;
    }

    responseStream.get();   // Remove ' ' from buffer
    std::getline(responseStream, statusMessage, '\r');
    responseStream.get();   // Remove symbol '\n' from the buffer.

    Log::Debug(Print::composeMessage("Status message:", statusMessage));

    mResponseMessage.setStatusCode(statusCode);
    mResponseMessage.setStatusMessage(statusMessage);

    if (mIsCanceled) {
        finish(boost::system::error_code(boost::asio::error::operation_aborted));
        return;
    }

    boost::asio::async_read_until(mSock, mResponseBuf,
        "\r\n\r\n",
        [this](const boost::system::error_code& ec, size_t bytesTransferred)
        {
            if (ec.value() != 0) {
                finish(ec);
                return;
            }
            else if (ec == boost::asio::error::eof) {
                Log::Debug("Eof, headers are absent");
            }
            readHeaders();
        }
    );
}

void Request::readHeaders()
{
    Log::Debug("Start response headers reading");

    std::istream requestStream(&mResponseBuf);
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
            mResponseMessage.addHeader(headerName, headerValue);
            Log::Debug(Print::composeMessage("Add header:", headerName, ":", headerValue));
        }
    }

    mResponseMessage.appendBody(std::string((std::istreambuf_iterator<char>(requestStream)), std::istreambuf_iterator<char>()));

    boost::asio::async_read(mSock, mResponseBuf,
        [this](const boost::system::error_code& ec, std::size_t bytes_transferred)
        {
            std::istream requestStream(&mResponseBuf);
            mResponseMessage.appendBody(std::string((std::istreambuf_iterator<char>(requestStream)), std::istreambuf_iterator<char>()));
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
    Log::Debug("Finish request");
    if (ec == boost::asio::error::operation_aborted) {
        Log::Info(Print::composeMessage("Request was canceled"));
    }
    else if (ec.value() != 0) {
        auto message = Print::composeMessage("Error occured! Error code =", ec.value(), ". Message:", ec.message(), "Request id:", mId);
        Log::Error(message);
        mResponseMessage.addHeader("Content-Length", "0");
        mResponseMessage.setStatusCode(StatusCode::ServerError);
        mCallback(mResponseMessage, mId);
        mSelfPtr.reset();
    }
    else {
        mCallback(mResponseMessage, mId);
        mSelfPtr.reset();
    }
}

} // namespace Http::Asio