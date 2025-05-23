#pragma once

#include <memory>
#include <functional>

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>

#include "Utils.h"
#include "HttpCommon.h"

namespace Http::Beast {

class Session;
using SessionPtr = std::shared_ptr<Session>;

class Session : public std::enable_shared_from_this<Session> {
public:
    using tcp = boost::asio::ip::tcp;
    using IoContext = boost::asio::io_context;
    using HttpResponse = boost::beast::http::response<boost::beast::http::string_body>;
    using Callback = std::function<void(const HttpResponse&, const Id& id)>;
public:
    Session(IoContext& ioc, const std::string& host, Port port, Callback callback = [](const HttpResponse&, const Id&) {});
    ~Session();
public:
    void get(const std::string& resource);
    void put(const std::string& resource, const std::string& body);
    void cancel();
public:
    const Id& getId() const;
    bool isCompleted();
    void addHeader(const std::string& name, const std::string& value);
private:
    void execute();
    void connect(tcp::resolver::results_type results);
    void sendRequest();
    void readResponse();
    void finish(boost::beast::error_code ec);
private:
    using Resolver = tcp::resolver;
    using TcpStream = boost::beast::tcp_stream;
    using FlatBuffer = boost::beast::flat_buffer;
    using HttpRequest = boost::beast::http::request<boost::beast::http::string_body>;
    using AtomicFlag = std::atomic<bool>;
private:
    Id mId;

    SessionPtr mSelfPtr;

    std::string mHost;
    Port mPort;

    Callback mCallback;

    AtomicFlag mIsCanceled;

    Resolver mResolver;
    TcpStream mStream;
    FlatBuffer mFlatBuffer;
    HttpRequest mRequest;
    HttpResponse mResponse;
};

} // namespace Http::Beast