#pragma once

#include "Singleton.h"

#include <boost/asio.hpp>

namespace Http::Asio {

using IoServiceBoost = boost::asio::io_service;

class IoService : public Singleton<IoService>
{
    friend class Singleton<IoService>;
public:
    IoServiceBoost& getIoService();
private:
    IoService() = default;
private:
    IoServiceBoost mIoService;
};

} // namespace Http::Asio