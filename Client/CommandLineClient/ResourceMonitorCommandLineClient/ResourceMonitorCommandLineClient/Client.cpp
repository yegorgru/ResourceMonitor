#include "Client.h"

namespace ResourceMonitorClient {

Client::Client()
    : mWork(mIoService)
    , mThread([this]() { mIoService.run(); })
{
}

void Client::emulateLongComputationOp(unsigned int durationSec, const std::string& rawIpAddress, Session::Port portNum, Session::Id requestId) {

    std::string request = "EMULATE_LONG_CALC_OP " + std::to_string(durationSec) + "\n";

    auto session = std::make_shared<Session>(mIoService, rawIpAddress, portNum, request, requestId, 
        [this](Session::Id request_id, const std::string& response, const Session::ErrorCode& ec) {
            handler(request_id, response, ec); 
        }
    );

    session->mSocket.open(session->mRemoteEndpoint.protocol());

    std::unique_lock<std::mutex> lock(mActiveSessionsGuard);
    mActiveSessions[requestId] = session;
    lock.unlock();

    session->mSocket.async_connect(session->mRemoteEndpoint,
        [this, session](const boost::system::error_code& ec)
        {
            if (ec.value() != 0) {
                session->mErrorCode = ec;
                onRequestComplete(session);
                return;
            }

            std::unique_lock<std::mutex> cancelLock(session->mCancelGuard);

            if (session->mWasCancelled) {
                onRequestComplete(session);
                return;
            }
            writeRequest(ec, session);
        }
    );
};

void Client::cancelRequest(Session::Id requestId) {
    std::unique_lock<std::mutex> lock(mActiveSessionsGuard);

    auto it = mActiveSessions.find(requestId);
    if (it != mActiveSessions.end()) {
        std::unique_lock<std::mutex> cancelLock(it->second->mCancelGuard);

        it->second->mWasCancelled = true;
        it->second->mSocket.cancel();
    }
}

void Client::close() {
    mIoService.stop();
    mThread.join();
}

void Client::onRequestComplete(SessionPtr session) {
    boost::system::error_code ignoredErrorCode;

    session->mSocket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignoredErrorCode);

    std::unique_lock<std::mutex> lock(mActiveSessionsGuard);

    auto it = mActiveSessions.find(session->mId);
    if (it != mActiveSessions.end()) {
        mActiveSessions.erase(it);
    }

    lock.unlock();

    boost::system::error_code ec = session->mErrorCode.value() == 0 && session->mWasCancelled ? 
        boost::asio::error::operation_aborted : session->mErrorCode;

    session->mCallback(session->mId, session->mResponse, ec);
};

void Client::writeRequest(const ErrorCode& ec, SessionPtr session) {
    boost::asio::async_write(session->mSocket, boost::asio::buffer(session->mRequest),
        [this, session](const boost::system::error_code& ec, size_t bytesTransferred)
        {
            if (ec.value() != 0) {
                session->mErrorCode = ec;
                onRequestComplete(session);
                return;
            }

            std::unique_lock<std::mutex> cancelLock(session->mCancelGuard);

            if (session->mWasCancelled) {
                onRequestComplete(session);
                return;
            }
            readResponse(ec, bytesTransferred, session);
        });
}

void Client::readResponse(const ErrorCode& ec, size_t bytesTransferred, SessionPtr session) {
    boost::asio::async_read_until(session->mSocket, session->mResponseBuf, '\n',
        [this, session](const boost::system::error_code& ec, size_t bytes_transferred)
        {
            if (ec.value() != 0) {
                session->mErrorCode = ec;
            }
            else {
                std::istream strm(&session->mResponseBuf);
                std::getline(strm, session->mResponse);
            }

            onRequestComplete(session);
        }
    );
}

void Client::handler(Session::Id request_id, const std::string& response, const Session::ErrorCode& ec) {
    if (ec.value() == 0) {
        std::cout << "Request #" << request_id
            << " has completed. Response: "
            << response << std::endl;
    }
    else if (ec == boost::asio::error::operation_aborted) {
        std::cout << "Request #" << request_id
            << " has been cancelled by the user."
            << std::endl;
    }
    else {
        std::cout << "Request #" << request_id
            << " failed! Error code = " << ec.value()
            << ". Error message = " << ec.message()
            << std::endl;
    }

    return;
}

} // namespace ResourceMonitorClient