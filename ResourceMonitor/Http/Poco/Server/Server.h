#pragma once

#include <memory>
#include <string>
#include <vector>
#include <thread>

#include <Poco/Net/HTTPServer.h>
#include <Poco/Net/HTTPServerParams.h>
#include <Poco/Net/ServerSocket.h>

#include "IServer.h"
#include "HttpCommon.h"

namespace Http::Poco {

class Server : public IServer {
public:
    Server();
    ~Server();
public:
    void start(Port portNum, unsigned int threadPoolSize) override;
    void stop() override;
    void configureDatabase(const std::string& dbName, int dbPort) override;
private:
    using PocoServer = ::Poco::Net::HTTPServer;
    using ServerPtr = std::unique_ptr<PocoServer>;
    using Socket = ::Poco::Net::ServerSocket;
    using SocketPtr = std::unique_ptr<Socket>;
private:
    ServerPtr mServer;
    SocketPtr mSocket;
    std::jthread mServerThread;
    bool mIsStarted;
};

} // namespace Http::Poco
