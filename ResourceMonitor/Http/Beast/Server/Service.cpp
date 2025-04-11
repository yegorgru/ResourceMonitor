#include "Service.h"
#include "Log.h"
#include "BoostCommon/DatabaseManager.h"
#include "HttpCommon.h"

#include <boost/algorithm/string/split.hpp>       
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include <map>
#include <set>
#include <deque>

namespace Http::Beast {

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
    : mStream(std::move(*socket))
{
}

void Service::startHandling() {
    mSelfPtr = shared_from_this();
    boost::asio::dispatch(mStream.get_executor(), [this](){
        mRequest = {};
        readRequest();
    });
}

void Service::readRequest() {
    namespace http = boost::beast::http;
    http::async_read(mStream, mBuffer, mRequest,
        [this](boost::beast::error_code ec, std::size_t) {
            if (ec == http::error::end_of_stream) {
                Log::Warning("End of stream, closing connection");
                mStream.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_both);
                finish();
                return;
            }
            else if (ec) {
                Log::Error(Print::composeMessage("Error occurred! Error code = ", ec.value(), ". Message: ", ec.message()));
                finish();
                return;
            }
            prepareResponse();
        }
    );
}   

void Service::prepareResponse() {
    namespace http = boost::beast::http;
    using AsioIoContext = boost::asio::io_context;
    using DatabaseManager = Boost::Common::DatabaseManager<AsioIoContext, Session>;
    auto method = mRequest.method();
    if (method == http::verb::put) {
        Log::Debug("PUT request processing");
        auto callback = [this](const HttpResponse& response, const Id& id) {
            auto status = response.result();
            if (status == http::status::ok) {
                Log::Debug(Print::composeMessage("Successfuly write info to database. Request:",id));
                sendResponse(status, "");
            }
            else {
                if (status == static_cast<boost::beast::http::status>(CANCELED_HTTP_STATUS)) {
                    status = boost::beast::http::status::internal_server_error;
                }
                Log::Error(Print::composeMessage("Error while writing info to database", response.result_int(), "Request:", id));
                sendResponse(status, response.body());
            }
        };
        DatabaseManager::Get().put(mRequest.target(), mRequest.body(), callback);
    }
    else if (method == http::verb::get) {
        Log::Debug("GET request processing");
        auto callback = [this](const HttpResponse& response, const Id& id) {
            auto status = response.result();
            if (status == http::status::ok) {
                Log::Debug("Successfuly get info from database");
                sendResponse(status, std::move(response.body()));
            }
            else {
                if (status == static_cast<boost::beast::http::status>(CANCELED_HTTP_STATUS)) {
                    status = boost::beast::http::status::internal_server_error;
                }
                Log::Error(Print::composeMessage("Error while getting info from database", response.result_int(), response.body()));
                sendResponse(status, response.body());
            }
        };
        DatabaseManager::Get().get(mRequest.target(), callback);
    }
}

void Service::sendResponse(boost::beast::http::status status, const std::string& response) {
    namespace http = boost::beast::http;    
    Log::Trace("Start sending response");

    mStream.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_receive);

    mResponse.result(status);
    mResponse.set(http::field::content_length, std::to_string(response.length()));
    mResponse.set(boost::beast::http::field::access_control_allow_origin, "*");
    mResponse.body() = response;

    http::async_write(mStream, std::move(mResponse), 
        [this](boost::beast::error_code ec, std::size_t) {
            if (ec) {
                Log::Error(Print::composeMessage("Error occurred! Error code = ", ec.value(), ". Message: ", ec.message()));
                finish();
                return;
            }
            mStream.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_send);
            finish();
        }
    );
}

void Service::finish() {
    Log::Trace("Finish service");
    mSelfPtr.reset();
}

} // namespace Http::Beast