#include "Server.h"
#include "RequestHandlerFactory.h"
#include "Log.h"
#include "Print.h"
#include "DatabaseManager.h"
#include "RequestHandlerFactory.h"

namespace Http::Poco {

Server::Server()
    : mServer(nullptr)
    , mSocket(nullptr)
    , mIsStarted(false)
{
}

Server::~Server()
{
    Log::Debug("Destroying Poco server");
    if (mIsStarted) {
        stop();
    }
}

void Server::start(Port portNum, const std::string& ipAddress, unsigned int threadPoolSize)
{
    if (mIsStarted) {
        Log::Warning("Poco server already started");
        return;
    }

    Log::Info("Starting Poco server");

    try {
        auto params = new ::Poco::Net::HTTPServerParams;
        params->setMaxQueued(600);
        params->setMaxThreads(threadPoolSize);
        params->setKeepAlive(true);

        ::Poco::Net::SocketAddress socketAddress(ipAddress, portNum);
        mSocket = std::make_unique<Socket>(socketAddress);
        auto factory = new RequestHandlerFactory;

        //params and factory are owned by the server
        mServer = std::make_unique<PocoServer>(factory, *mSocket, params);

        mServerThread = std::jthread([this]() {
            try {
                mServer->start();
            }
            catch (const std::exception& e) {
                Log::Error(Print::composeMessage("Error in server thread:", e.what()));
            }
        });

        mIsStarted = true;

        Log::Info(Print::composeMessage("Poco server started on", ipAddress, ":", portNum, "with", threadPoolSize, "threads"));
    }
    catch (const std::exception& e) {
        Log::Error(Print::composeMessage("Error starting Poco server:", e.what()));
        stop();
        throw;
    }
}

void Server::stop()
{
    if (!mIsStarted) {
        return;
    }

    Log::Info("Stopping Poco server");
    
    if (mServer) {
        mServer->stop();
        
        if (mServerThread.joinable()) {
            mServerThread.join();
        }
        
        mServer.reset();
    }
    
    mSocket.reset();
    
    mIsStarted = false;
    Log::Info("Poco server stopped");
}

void Server::configureDatabase(const std::string& dbName, int dbPort)
{
    DatabaseManager::Destroy();
    DatabaseManager::Init(dbName, dbPort);
    Log::Debug(Print::composeMessage("Poco server database configured:", "name =", dbName, "port =", dbPort));
}

} // namespace Http::Poco
