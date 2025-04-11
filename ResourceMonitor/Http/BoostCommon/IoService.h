#pragma once

#include "Singleton.h"

#include <boost/asio.hpp>

namespace Http::Boost::Common {

template <typename IoServiceType>
class IoService : public Singleton<IoService<IoServiceType>>
{
    friend class Singleton<IoService<IoServiceType>>;
public:
    IoServiceType& getIoService();
private:
    IoService() = default;
private:
    IoServiceType mIoService;
};

template <typename IoServiceType>
IoServiceType& IoService<IoServiceType>::getIoService() {
	return mIoService;
}

} // namespace Http::Boost::Common