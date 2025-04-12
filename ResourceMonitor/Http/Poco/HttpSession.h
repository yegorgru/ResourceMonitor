#pragma once

#include <memory>
#include <functional>
#include <string>
#include <atomic>
#include <future>
#include <map>

#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>

#include "Utils.h"
#include "HttpCommon.h"

namespace Http::Poco {

class Session;
using SessionPtr = std::shared_ptr<Session>;

class Session : public std::enable_shared_from_this<Session> {
public:
    struct ResponseData {
        std::string mBody;
        StatusCode mStatusCode;
        std::string mStatusMessage;
        
        ResponseData(const std::string& body, StatusCode code, const std::string& message);
    };

    using Callback = std::function<void(const ResponseData& response, const Id& id)>;
    
public:
    Session(const std::string& host, Port port, Callback callback = [](const ResponseData&, const Id&) {});
    ~Session();
    
public:
    void get(const std::string& endpoint);
    void put(const std::string& endpoint, const std::string& body);
    void cancel();
    void addHeader(const std::string& name, const std::string& value);
    void setTimeout(int timeoutSeconds);
    void setUseThreadPool(bool useThreadPool);
    
public:
    bool isCompleted() const;
    const Id& getId() const;
    
private:
    void execute();
    void finish(bool success, const std::string& errorMessage = "");
    
private:
    using HttpClientSession = ::Poco::Net::HTTPClientSession;
    using HttpRequest = ::Poco::Net::HTTPRequest;
    using HttpResponse = ::Poco::Net::HTTPResponse;
    using HeaderMap = std::map<std::string, std::string>;
    using AtomicFlag = std::atomic<bool>;
    
private:
    Id mId;
    std::string mBody;
    Callback mCallback;
    HeaderMap mHeaders;
    
    HttpClientSession mSession;
    HttpRequest mRequest;
    HttpResponse mResponse;

    AtomicFlag mCompleted;
    AtomicFlag mCanceled;
    bool mUseThreadPool;
    
    SessionPtr mSelfPtr;
};

} // namespace Http::Poco
