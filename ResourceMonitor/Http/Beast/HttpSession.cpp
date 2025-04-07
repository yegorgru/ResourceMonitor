#include "HttpSession.h"
#include "Log.h"
#include "Utils.h"
#include "HttpCommons.h"

#include <boost/asio/strand.hpp>
#include <boost/beast/version.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace Http::Beast {

namespace http = boost::beast::http;
namespace beast = boost::beast;

Session::Session(IoContext& ioc, const std::string& host, unsigned int port, Callback callback) 
    : mId(Commons::generateId())
    , mHost(host)
    , mPort(port)
    , mCallback(callback)
    , mIsCanceled(false)
    , mResolver(boost::asio::make_strand(ioc))
    , mStream(boost::asio::make_strand(ioc)) 
{
}

Session::~Session() {
    Log::Trace("Session destructor");
}

void Session::get(const std::string& resource) {
    mRequest.method(http::verb::get);
    mRequest.target(resource);
    mRequest.set(http::field::host, mHost);
    mRequest.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
    execute();  
}

void Session::put(const std::string& resource, const std::string& body) {
    mRequest.method(http::verb::put);
    mRequest.target(resource);
    mRequest.set(http::field::host, mHost);
    mRequest.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
    mRequest.set(http::field::content_type, "application/json");
    mRequest.body() = body;
    mRequest.prepare_payload();
    execute();
}

void Session::addHeader(boost::beast::http::field name, const std::string& value) {
    mRequest.set(name, value);
}

void Session::execute() {
    if (!isValidPort(mPort) || mHost.empty() || mRequest.target().empty()) {
        Log::Throw(Print::composeMessage("Incorrect request parameters. Port:", mPort, "host:", mHost, "resource:", mRequest.target()));
    }
    Log::Debug(Print::composeMessage("Start request executing. Port:", mPort, "host:", mHost, "resource:", mRequest.target()));
    mSelfPtr = shared_from_this();
    mResolver.async_resolve(mHost, std::to_string(mPort),
        [this](beast::error_code ec, tcp::resolver::results_type results) {
            if (ec.value() != 0) {
                finish(ec);
                return;
            }
            connect(std::move(results));
        }
    );
}

void Session::connect(tcp::resolver::results_type results) {
    if(mTimeout) {
        mStream.expires_after(std::chrono::seconds(*mTimeout));
    }
    mStream.async_connect(results, 
        [this](beast::error_code ec, tcp::resolver::results_type::endpoint_type) {
            if (ec.value() != 0) {
                finish(ec);
                return;
            }
            sendRequest();
        }
    );
}

void Session::sendRequest() {
    if (mTimeout) {
        mStream.expires_after(std::chrono::seconds(*mTimeout));
    }
    http::async_write(mStream, mRequest,
        [this](beast::error_code ec, std::size_t) {
            if (ec.value() != 0) {
                finish(ec);
                return;
            }
            readResponse();
        }
    );
}

void Session::readResponse() {
    http::async_read(mStream, mFlatBuffer, mResponse,
        [this](beast::error_code ec, std::size_t) {
            if (ec.value() != 0) {
                finish(ec);
                return;
            }
            mStream.socket().shutdown(tcp::socket::shutdown_both);        
            finish(boost::system::error_code{});
        }
    );
}

void Session::finish(beast::error_code ec) {
    Log::Debug(Print::composeMessage("Finish request", boost::uuids::to_string(mId)));
    if (ec == boost::asio::error::operation_aborted) {
        Log::Info(Print::composeMessage("Request was canceled"));
    }
    else if (ec.value() != 0) {
        auto message = Print::composeMessage("Error occured! Error code =", ec.value(), ". Message:", ec.message(), "Request id:", boost::uuids::to_string(mId));
        Log::Error(message);
        mResponse.set(http::field::content_length, "0");
        mResponse.result(http::status::internal_server_error);
        mCallback(mResponse, mId);
        mSelfPtr.reset();
    }
    else {
        mCallback(mResponse, mId);
        mSelfPtr.reset();
    }    
}

void Session::setTimeout(int seconds) {
    mTimeout = seconds;
}

bool Session::isCompleted() {
    return mSelfPtr == nullptr;
}

const Session::Id& Session::getId() const {
    return mId;
}

void Session::cancel() {
    if (isCompleted()) {
        return;
    }
    Log::Debug(Print::composeMessage("Cancelling request. Id:", boost::uuids::to_string(mId)));
    mResponse.result(static_cast<boost::beast::http::status>(CANCELED_HTTP_STATUS));
    mCallback(mResponse, mId);
    mIsCanceled = true;
    mResolver.cancel();
    if (mStream.socket().is_open()) {
        mStream.socket().cancel();
    }
    mSelfPtr.reset();
}

}