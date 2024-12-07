#include "Client.h"
#include "Log.h"

#include <boost/uuid/uuid_io.hpp>
#include <nlohmann/json.hpp>

namespace ResourceMonitorClient {

Client::Client()
    : mWork(boost::asio::make_work_guard(mIoService))
{
    mThreadIo = std::jthread([this]() { 
        mIoService.run();
    });
}

Client::~Client()
{
    LOG::Debug("Destroying client");
    if (mWork.owns_work()) {
        close();
    }
}

Client::OptionalRequestId Client::makeRequest(int serverPort, const std::string& serverName, const std::string& resource, const std::string& count, const std::string& ipAddress) {
    LOG::Debug("Making request");
    auto callback = getCallback(resource);
    if (!callback) {
        return std::nullopt;
    }
    auto request = std::make_shared<Http::Request>(mIoService, serverName, serverPort, *callback);
    std::string endpoint = resource + "/" + count + "/" + ipAddress;
    request->get(endpoint);
    const auto& id = request->getId();
    mRequests[id] = request;
    return boost::uuids::to_string(id);
}

void Client::cancelRequest(const std::string strId) {
    LOG::Debug("Canceling request");
    static boost::uuids::string_generator stringGen;
    std::string message;
    try
    {
        Http::Request::Id id = stringGen(strId);
        std::lock_guard<std::mutex> lg(mRequestsMutex);
        auto found = mRequests.find(id);
        if (found != mRequests.end()) {
            if (!found->second->isCompleted()) {
                found->second->cancel();
                message = LOG::composeMessage("Request canceled. Id:", strId);
                LOG::Info(message);
                mRequests.erase(found);
                LOG::Debug(LOG::composeMessage("Canceled request removed from storage. Id:", boost::uuids::to_string(id)));
            }
            else {
                message = LOG::composeMessage("Error: completed request still in storage. Id:", strId);
                LOG::Error(message);
            }
        }
        else {
            message = LOG::composeMessage("No requests with such id in storage. Id:", strId);
            LOG::Info(message);
        }
    }
    catch (const std::runtime_error&)
    {
        message = LOG::composeMessage("Failed to convert string to valid id. Id:", strId);
        LOG::Error(message);
    }
    LOG::SyncPrintLine(message, std::cout);
}

void Client::close() {
    LOG::Debug("Client close");
    mWork.reset();
    mThreadIo.join();
}

Client::OptionalCallback Client::getCallback(const std::string& resource) {
    using json = nlohmann::json;
    static const std::map<std::string, std::function<void(const nlohmann::json& parsedJson)>> callbacks = {
        {
            "basic_info",
            [this](const nlohmann::json& parsedJson) {
                LOG::SyncPrintLine(LOG::composeMessage("Basic info for", parsedJson["machineIp"]), std::cout);
                for (const auto& row : parsedJson["rows"]) {
                    LOG::SyncPrintLine("==================================", std::cout);
                    LOG::SyncPrintLine(LOG::composeMessage("Timestamp:", parsedJson["timestamp"]), std::cout);
                    LOG::SyncPrintLine(LOG::composeMessage("Cpu count:", parsedJson["numcpus"]), std::cout);
                    LOG::SyncPrintLine(LOG::composeMessage("Total virtual memory (GB):", parsedJson["total virt mem GB"]), std::cout);
                    LOG::SyncPrintLine(LOG::composeMessage("Total swap memory (GB):", parsedJson["total swap mem GB"]), std::cout);
                    LOG::SyncPrintLine(LOG::composeMessage("Disk partitions count:", parsedJson["numdisks"]), std::cout);
                    LOG::SyncPrintLine(LOG::composeMessage("Total disk (GB):", parsedJson["total_C_disk GB"]), std::cout);
                    LOG::SyncPrintLine("==================================", std::cout);
                }
            }
        },
        {
            "cpu",
            [this](const nlohmann::json& parsedJson) {
                LOG::SyncPrintLine(LOG::composeMessage("Cpu info for", parsedJson["machineIp"]), std::cout);
                for (const auto& row : parsedJson["rows"]) {
                    LOG::SyncPrintLine("==================================", std::cout);
                    LOG::SyncPrintLine(LOG::composeMessage("Timestamp:", parsedJson["timestamp"]), std::cout);
                    LOG::SyncPrintLine(LOG::composeMessage("Cpu usage (%):", parsedJson["cpu_usage %"]), std::cout);
                    LOG::SyncPrintLine(LOG::composeMessage("Time spent by normal processes executing in user mode:", parsedJson["cpu_times"]["cpu_user"]), std::cout);
                    LOG::SyncPrintLine(LOG::composeMessage("Time spent by processes executing in kernel mode:", parsedJson["cpu_times"]["cpu_system"]), std::cout);
                    LOG::SyncPrintLine(LOG::composeMessage("Idle time:", parsedJson["cpu_times"]["cpu_idle"]), std::cout);
                    LOG::SyncPrintLine(LOG::composeMessage("Current cpu frequency Mhz:", parsedJson["cpu_freq"]["freq_curr Mhz"]), std::cout);
                    LOG::SyncPrintLine("==================================", std::cout);
                }
            }
        },
        {
            "memory",
            [this](const nlohmann::json& parsedJson) {
                LOG::SyncPrintLine(LOG::composeMessage("Basic info for", parsedJson["machineIp"]), std::cout);
                for (const auto& row : parsedJson["rows"]) {
                    LOG::SyncPrintLine("==================================", std::cout);
                    LOG::SyncPrintLine(LOG::composeMessage("Timestamp:", parsedJson["timestamp"]), std::cout);
                    LOG::SyncPrintLine(LOG::composeMessage("Virtual memory usage (%):", parsedJson["virt_memory"]["usage virt %"]), std::cout);
                    LOG::SyncPrintLine(LOG::composeMessage("Used virtual memory (GB):", parsedJson["virt_memory"]["used virt GB"]), std::cout);
                    LOG::SyncPrintLine(LOG::composeMessage("Available virtual memory (GB):", parsedJson["virt_memory"]["available virt GB"]), std::cout);
                    LOG::SyncPrintLine(LOG::composeMessage("Swap memory usage (%):", parsedJson["swap_memory"]["usage swap %"]), std::cout);
                    LOG::SyncPrintLine(LOG::composeMessage("Used swap memory (GB):", parsedJson["swap_memory"]["used swap GB"]), std::cout);
                    LOG::SyncPrintLine(LOG::composeMessage("Free swap memory (GB):", parsedJson["virt_memory"]["available virt GB"]), std::cout);
                    LOG::SyncPrintLine("==================================", std::cout);
                }
            }
        },
        {
            "disks",
            [this](const nlohmann::json& parsedJson) {
                LOG::SyncPrintLine(LOG::composeMessage("Basic info for", parsedJson["machineIp"]), std::cout);
                for (const auto& row : parsedJson["rows"]) {
                    LOG::SyncPrintLine("==================================", std::cout);
                    LOG::SyncPrintLine(LOG::composeMessage("Timestamp:", parsedJson["timestamp"]), std::cout);
                    LOG::SyncPrintLine(LOG::composeMessage("Disk usage (%):", parsedJson["disk usage"]["usage %"]), std::cout);
                    LOG::SyncPrintLine(LOG::composeMessage("Used disk (GB):", parsedJson["disk usage"]["used GB"]), std::cout);
                    LOG::SyncPrintLine(LOG::composeMessage("Free disk (GB):", parsedJson["disk usage"]["free GB"]), std::cout);
                    LOG::SyncPrintLine(LOG::composeMessage("Disk i/o read count:", parsedJson["disk i/o"]["read_count"]), std::cout);
                    LOG::SyncPrintLine(LOG::composeMessage("Disk i/o write count:", parsedJson["disk i/o"]["read_count"]), std::cout);
                    LOG::SyncPrintLine(LOG::composeMessage("Number of GB read:", parsedJson["disk i/o"]["read_bytes GB"]), std::cout);
                    LOG::SyncPrintLine(LOG::composeMessage("Number of GB written:", parsedJson["disk i/o"]["write_bytes GB"]), std::cout);
                    LOG::SyncPrintLine("==================================", std::cout);
                }
            }
        },
        {
            "network",
            [this](const nlohmann::json& parsedJson) {
                LOG::SyncPrintLine(LOG::composeMessage("Basic info for", parsedJson["machineIp"]), std::cout);
                for (const auto& row : parsedJson["rows"]) {
                    LOG::SyncPrintLine("==================================", std::cout);
                    LOG::SyncPrintLine(LOG::composeMessage("Timestamp:", parsedJson["timestamp"]), std::cout);
                    LOG::SyncPrintLine(LOG::composeMessage("Number of connections:", parsedJson["connections"]), std::cout);
                    LOG::SyncPrintLine(LOG::composeMessage("Number of packets sent:", parsedJson["i/o"]["packets_sent"]), std::cout);
                    LOG::SyncPrintLine(LOG::composeMessage("Number of packets received:", parsedJson["i/o"]["packets_recv"]), std::cout);
                    LOG::SyncPrintLine(LOG::composeMessage("Number of bytes sent:", parsedJson["i/o"]["bytes_sent"]), std::cout);
                    LOG::SyncPrintLine(LOG::composeMessage("Number of bytes received:", parsedJson["i/o"]["bytes_recv"]), std::cout);
                    LOG::SyncPrintLine(LOG::composeMessage("Total number of errors while receiving:", parsedJson["i/o"]["errors in"]), std::cout);
                    LOG::SyncPrintLine(LOG::composeMessage("Total number of errors while sending:", parsedJson["i/o"]["errors out"]), std::cout);
                    LOG::SyncPrintLine(LOG::composeMessage("Total number of incoming packets which were dropped:", parsedJson["i/o"]["pack drop in"]), std::cout);
                    LOG::SyncPrintLine(LOG::composeMessage("Total number of outgoing packets which were dropped:", parsedJson["i/o"]["pack drop out"]), std::cout);
                    LOG::SyncPrintLine("==================================", std::cout);
                }
            }
        },
    };
    auto found = callbacks.find(resource);
    if (found == callbacks.end()) {
        return std::nullopt;
    }
    else {
        return [this, found](const Http::MessageResponse& response, const Http::Request::Id& id) {
            auto statusCode = response.getStatusCode();
            if (statusCode == Http::StatusCode::ClientClosedRequest) {
                LOG::Debug("Callback for canceled request");
                return;
            }
            else {
                std::lock_guard<std::mutex> lg(mRequestsMutex);
                mRequests.erase(id);
                LOG::Debug(LOG::composeMessage("Completed request removed from storage. Id:", boost::uuids::to_string(id)));
            }
            std::string message;
            if (statusCode == Http::StatusCode::Ok) {
                const auto& jsonStr = response.getBody();
                auto parsedJson = json::parse(jsonStr);
                if (!parsedJson.empty() && !parsedJson.contains("error")) {

                    found->second(parsedJson);

                    message = "Request processed successfully";
                    LOG::Info(message);
                }
                else if (parsedJson.empty()) {
                    message = LOG::composeMessage("Failed to parse json response", jsonStr);
                    LOG::Error(message);
                }
                else {
                    message = LOG::composeMessage("Error message from server", parsedJson["error"]);
                    LOG::Error(message);
                }
            }
            else {
                message = LOG::composeMessage("Failed to get info from server", static_cast<int>(statusCode), response.getStatusMessage(), response.getBody());
                LOG::Error(message);
            }
            LOG::SyncPrintLine(message, std::cout);
            auto finishMessage = LOG::composeMessage("Finished request:", boost::uuids::to_string(id));
            LOG::Info(finishMessage);
            LOG::SyncPrintLine(finishMessage, std::cout);
        };
    }
}

} // namespace ResourceMonitorClient