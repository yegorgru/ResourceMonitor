#pragma once

#include <memory>
#include <string>

namespace Http {

class IServer {
public:
    using Port = unsigned int;
public:
    IServer() = default;
    virtual ~IServer() = default;
public:
    virtual void start(Port portNum, unsigned int threadPoolSize) = 0;
    virtual void stop() = 0;
    virtual void configureDatabase(const std::string& dbName, int dbPort) = 0;
};

using ServerPtr = std::unique_ptr<IServer>;

} // namespace Http::Asio