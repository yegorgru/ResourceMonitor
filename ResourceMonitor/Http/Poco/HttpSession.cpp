#include "HttpSession.h"
#include "Log.h"
#include "Print.h"
#include "ThreadPoolManager.h"

#include <Poco/StreamCopier.h>
#include <Poco/Exception.h>
#include <Poco/JSON/Parser.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>

namespace Http::Poco {

Session::Session(const std::string& host, Port port, Callback callback)    
    : mId(generateId())
    , mCallback(callback)
    // known issue with localhost for Poco
    , mSession(host == "localhost" ? "127.0.0.1" : host, port)
    , mCompleted(false)
    , mCanceled(false)
    , mUseThreadPool(true)
{
    Log::Debug(Print::composeMessage("Created HTTP session. ID:", idToString(mId), "Host:", host, "Port:", port));
}

Session::~Session()
{
    Log::Trace("Session destructor");
    if (!mCompleted && !mCanceled) {
        Log::Warning(Print::composeMessage("Session destroyed without being completed or canceled. ID:", idToString(mId)));
        cancel();
    }
}

void Session::get(const std::string& endpoint)
{
    mRequest = HttpRequest(HttpRequest::HTTPRequest::HTTP_GET, endpoint, ::Poco::Net::HTTPMessage::HTTP_1_1);
    
    for (const auto& [name, value] : mHeaders) {
        mRequest.set(name, value);
    }
    
    execute();
}

void Session::put(const std::string& endpoint, const std::string& body)
{
    mBody = body;
    
    mRequest = HttpRequest(HttpRequest::HTTP_PUT, endpoint, ::Poco::Net::HTTPMessage::HTTP_1_1);
    
    for (const auto& [name, value] : mHeaders) {
        mRequest.set(name, value);
    }
    mRequest.setContentLength(body.length());
    
    execute();
}

void Session::addHeader(const std::string& name, const std::string& value)
{
    mHeaders[name] = value;
}

void Session::setTimeout(int timeoutSeconds)
{
    mSession.setTimeout(::Poco::Timespan(timeoutSeconds, 0));
}

void Session::setUseThreadPool(bool useThreadPool)
{
    mUseThreadPool = useThreadPool;
    Log::Debug(Print::composeMessage("Session", idToString(mId), "thread pool usage set to:", useThreadPool));
}

void Session::execute()
{    
    mSelfPtr = shared_from_this();
    
    auto executeSession = [self = shared_from_this()]() {
        try {            
            if (self->mCanceled) {
                self->finish(false, "Request was canceled before execution");
                return;
            }
            
            if (self->mRequest.getMethod() == HttpRequest::HTTP_PUT) {
                std::ostream& requestStream = self->mSession.sendRequest(self->mRequest);
                requestStream << self->mBody;
            } else {
                self->mSession.sendRequest(self->mRequest);
            }
            
            if (self->mCanceled) {
                self->finish(false, "Request was canceled during sending");
                return;
            }
            
            std::istream& responseStream = self->mSession.receiveResponse(self->mResponse);
            
            std::string responseBody;
            ::Poco::StreamCopier::copyToString(responseStream, responseBody);
            
            ResponseData response(
                responseBody,
                static_cast<StatusCode>(self->mResponse.getStatus()),
                self->mResponse.getReason()
            );
            
            self->mCallback(response, self->mId);
            self->finish(true);
        }
        catch (const ::Poco::Exception& ex) {
            if (self->mCanceled) {
                self->finish(false, "Request was canceled");
            } else {
                std::string errorMsg = Print::composeMessage("Poco exception:", ex.displayText());
                Log::Error(errorMsg);
                self->finish(false, errorMsg);
            }
        }
        catch (const std::exception& ex) {
            if (self->mCanceled) {
                self->finish(false, "Request was canceled");
            } else {
                std::string errorMsg = Print::composeMessage("Standard exception:", ex.what());
                Log::Error(errorMsg);
                self->finish(false, errorMsg);
            }
        }
        catch (...) {
            if (self->mCanceled) {
                self->finish(false, "Request was canceled");
            } else {
                std::string errorMsg = "Unknown exception in HTTP session execution";
                Log::Error(errorMsg);
                self->finish(false, errorMsg);
            }
        }
    };
    
    if (mUseThreadPool) {
        ThreadPoolManager::Get().enqueue(executeSession);
    } else {
        executeSession();
    }
}

void Session::finish(bool success, const std::string& errorMessage)
{
    if (mCompleted) {
        return;
    }
    
    Log::Debug(
        Print::composeMessage(
            "Finishing request. Success:", success, 
            "Error:", errorMessage.empty() ? "none" : errorMessage,
            "ID:", idToString(mId)
        )
    );
    
    if (!success && !mCanceled) {
        ResponseData errorResponse("", StatusCode::ServerError, errorMessage);
        mCallback(errorResponse, mId);
    }
    
    mCompleted = true;
    mSelfPtr.reset();
}

void Session::cancel()
{
    if (mCompleted || mCanceled) {
        return;
    }
    
    Log::Debug(Print::composeMessage("Canceling request with ID:", idToString(mId)));
    mCanceled = true;
    
    try {
        mSession.abort();
    }
    catch (const ::Poco::Exception& ex) {
        Log::Debug(Print::composeMessage("Exception while aborting session:", ex.displayText()));
    }
    
    if (!mCompleted) {
        ResponseData cancelResponse("", StatusCode::ClientClosedRequest, "Request Canceled");
        mCallback(cancelResponse, mId);
        mCompleted = true;
        mSelfPtr.reset();
    }
}

bool Session::isCompleted() const
{
    return mCompleted;
}

const Id& Session::getId() const
{
    return mId;
}

Session::ResponseData::ResponseData(const std::string& body, StatusCode code, const std::string& message)
    : mBody(body)
    , mStatusCode(code)
    , mStatusMessage(message) 
{
}

} // namespace Http::Poco