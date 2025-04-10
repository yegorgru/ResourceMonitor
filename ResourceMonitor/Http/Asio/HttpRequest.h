#pragma once

#include "HttpMessage.h"
#include "HttpCommons.h"
#include "Utils.h"

#include <functional>

#include <boost/asio.hpp>

namespace Http::Asio {

class Request;
using RequestPtr = std::shared_ptr<Request>;

class Request : public std::enable_shared_from_this<Request> {
public:
    using IoService = boost::asio::io_service;
    using Port = unsigned int;
    using Callback = std::function<void(const MessageResponse&, const Id& id)>;
public:
    static const Port DEFAULT_PORT = 80;
public:
    Request(IoService& ios, const std::string& host, unsigned int port, Callback callback = [](const MessageResponse&, const Id&) {});
    ~Request();
public:
    void get(const std::string& resource);
    void put(const std::string& resource, const std::string& body);
public:
    void cancel();
public:
    const Id& getId() const;
    bool isCompleted();
    void addHeader(const std::string& name, const std::string& value);
private:
    void execute();
    void connect(boost::asio::ip::tcp::resolver::iterator iterator);
    void sendRequest();
    void readResponse();
    void readStatusLine();
    void readHeaders();
    void finish(const boost::system::error_code& ec);
private:
    using TcpSocket = boost::asio::ip::tcp::socket;
    using TcpResolver = boost::asio::ip::tcp::resolver;
    using AtomicFlag = std::atomic<bool>;
    using ResponseBuf = boost::asio::streambuf;
    using ResponseStream = std::istream;
private:
    Id mId;

    RequestPtr mSelfPtr;

    MessageRequest mRequestMessage;
    MessageResponse mResponseMessage;

    std::string mHost;
    unsigned int mPort;

    Callback mCallback;

    TcpSocket mSock;
    TcpResolver mResolver;

    ResponseBuf mResponseBuf;

    AtomicFlag mIsCanceled;

    IoService& mIoService;
};

using RequestPtr = std::shared_ptr<Request>;

} // namespace Http::Asio