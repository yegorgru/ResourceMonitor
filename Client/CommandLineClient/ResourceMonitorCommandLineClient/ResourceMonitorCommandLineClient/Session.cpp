#include "Session.h"

namespace ResourceMonitorClient {

Session::Session(IoService& ios, const std::string& rawIpAddress, Port port_num, const std::string& request, Id id, Callback callback)
    : mSocket(ios)
    , mRemoteEndpoint(boost::asio::ip::address::from_string(rawIpAddress), port_num)
    , mRequest(request)
    , mId(id)
    , mCallback(callback)
    , mWasCancelled(false)
{
}

} // namespace ResourceMonitorClient