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
                message = PRINT::composeMessage("Request canceled. Id:", strId);
                LOG::Info(message);
                mRequests.erase(found);
                LOG::Debug(PRINT::composeMessage("Canceled request removed from storage. Id:", boost::uuids::to_string(id)));
            }
            else {
                message = PRINT::composeMessage("Error: completed request still in storage. Id:", strId);
                LOG::Error(message);
            }
        }
        else {
            message = PRINT::composeMessage("No requests with such id in storage. Id:", strId);
            LOG::Info(message);
        }
    }
    catch (const std::runtime_error&)
    {
        message = PRINT::composeMessage("Failed to convert string to valid id. Id:", strId);
        LOG::Error(message);
    }
    PRINT::PrintLine(message, std::cout);
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
                PRINT::PrintLine(PRINT::composeMessage("Basic info for", parsedJson["machineIp"]), std::cout);
                for (const auto& row : parsedJson["rows"]) {
                    LOG::Debug(PRINT::composeMessage(parsedJson.dump()));
                    PRINT::PrintLine("==================================", std::cout);
                    PRINT::PrintLine(PRINT::composeMessage("Timestamp:", row["timestamp"]), std::cout);
                    PRINT::PrintLine(PRINT::composeMessage("Cpu count:", row["numcpus"]), std::cout);
                    PRINT::PrintLine(PRINT::composeMessage("Total virtual memory (GB):", row["total virt mem GB"]), std::cout);
                    PRINT::PrintLine(PRINT::composeMessage("Total swap memory (GB):", row["total swap mem GB"]), std::cout);
                    PRINT::PrintLine(PRINT::composeMessage("Disk partitions count:", row["numdisks"]), std::cout);
                    PRINT::PrintLine(PRINT::composeMessage("Total disk (GB):", row["total_C_disk GB"]), std::cout);
                    PRINT::PrintLine("==================================", std::cout);
                }
            }
        },
        {
            "cpu",
            [this](const nlohmann::json& parsedJson) {
                PRINT::PrintLine(PRINT::composeMessage("Cpu info for", parsedJson["machineIp"]), std::cout);
                for (const auto& row : parsedJson["rows"]) {
                    PRINT::PrintLine("==================================", std::cout);
                    PRINT::PrintLine(PRINT::composeMessage("Timestamp:", row["timestamp"]), std::cout);
                    PRINT::PrintLine(PRINT::composeMessage("Cpu usage (%):", row["cpu_usage %"]), std::cout);
                    PRINT::PrintLine(PRINT::composeMessage("Time spent by normal processes executing in user mode:", row["cpu_times"]["cpu_user"]), std::cout);
                    PRINT::PrintLine(PRINT::composeMessage("Time spent by processes executing in kernel mode:", row["cpu_times"]["cpu_system"]), std::cout);
                    PRINT::PrintLine(PRINT::composeMessage("Idle time:", row["cpu_times"]["cpu_idle"]), std::cout);
                    PRINT::PrintLine(PRINT::composeMessage("Current cpu frequency Mhz:", row["cpu_freq"]["freq_curr Mhz"]), std::cout);
                    PRINT::PrintLine("==================================", std::cout);
                }
            }
        },
        {
            "memory",
            [this](const nlohmann::json& parsedJson) {
                PRINT::PrintLine(PRINT::composeMessage("Memory info for", parsedJson["machineIp"]), std::cout);
                for (const auto& row : parsedJson["rows"]) {
                    PRINT::PrintLine("==================================", std::cout);
                    PRINT::PrintLine(PRINT::composeMessage("Timestamp:", row["timestamp"]), std::cout);
                    PRINT::PrintLine(PRINT::composeMessage("Virtual memory usage (%):", row["virt_memory"]["usage virt %"]), std::cout);
                    PRINT::PrintLine(PRINT::composeMessage("Used virtual memory (GB):", row["virt_memory"]["used virt GB"]), std::cout);
                    PRINT::PrintLine(PRINT::composeMessage("Available virtual memory (GB):", row["virt_memory"]["available virt GB"]), std::cout);
                    PRINT::PrintLine(PRINT::composeMessage("Swap memory usage (%):", row["swap_memory"]["usage swap %"]), std::cout);
                    PRINT::PrintLine(PRINT::composeMessage("Used swap memory (GB):", row["swap_memory"]["used swap GB"]), std::cout);
                    PRINT::PrintLine(PRINT::composeMessage("Free swap memory (GB):", row["virt_memory"]["available virt GB"]), std::cout);
                    PRINT::PrintLine("==================================", std::cout);
                }
            }
        },
        {
            "disks",
            [this](const nlohmann::json& parsedJson) {
                PRINT::PrintLine(PRINT::composeMessage("Disks info for", parsedJson["machineIp"]), std::cout);
                for (const auto& row : parsedJson["rows"]) {
                    PRINT::PrintLine("==================================", std::cout);
                    PRINT::PrintLine(PRINT::composeMessage("Timestamp:", row["timestamp"]), std::cout);
                    PRINT::PrintLine(PRINT::composeMessage("Disk usage (%):", row["disk usage"]["usage %"]), std::cout);
                    PRINT::PrintLine(PRINT::composeMessage("Used disk (GB):", row["disk usage"]["used GB"]), std::cout);
                    PRINT::PrintLine(PRINT::composeMessage("Free disk (GB):", row["disk usage"]["free GB"]), std::cout);
                    PRINT::PrintLine(PRINT::composeMessage("Disk i/o read count:", row["disk i/o"]["read_count"]), std::cout);
                    PRINT::PrintLine(PRINT::composeMessage("Disk i/o write count:", row["disk i/o"]["read_count"]), std::cout);
                    PRINT::PrintLine(PRINT::composeMessage("Number of GB read:", row["disk i/o"]["read_bytes GB"]), std::cout);
                    PRINT::PrintLine(PRINT::composeMessage("Number of GB written:", row["disk i/o"]["write_bytes GB"]), std::cout);
                    PRINT::PrintLine("==================================", std::cout);
                }
            }
        },
        {
            "network",
            [this](const nlohmann::json& parsedJson) {
                PRINT::PrintLine(PRINT::composeMessage("Network info for", parsedJson["machineIp"]), std::cout);
                for (const auto& row : parsedJson["rows"]) {
                    PRINT::PrintLine("==================================", std::cout);
                    PRINT::PrintLine(PRINT::composeMessage("Timestamp:", row["timestamp"]), std::cout);
                    PRINT::PrintLine(PRINT::composeMessage("Number of connections:", row["connections"]), std::cout);
                    PRINT::PrintLine(PRINT::composeMessage("Number of packets sent:", row["i/o"]["packets_sent"]), std::cout);
                    PRINT::PrintLine(PRINT::composeMessage("Number of packets received:", row["i/o"]["packets_recv"]), std::cout);
                    PRINT::PrintLine(PRINT::composeMessage("Number of bytes sent:", row["i/o"]["bytes_sent"]), std::cout);
                    PRINT::PrintLine(PRINT::composeMessage("Number of bytes received:", row["i/o"]["bytes_recv"]), std::cout);
                    PRINT::PrintLine(PRINT::composeMessage("Total number of errors while receiving:", row["i/o"]["errors in"]), std::cout);
                    PRINT::PrintLine(PRINT::composeMessage("Total number of errors while sending:", row["i/o"]["errors out"]), std::cout);
                    PRINT::PrintLine(PRINT::composeMessage("Total number of incoming packets which were dropped:", row["i/o"]["pack drop in"]), std::cout);
                    PRINT::PrintLine(PRINT::composeMessage("Total number of outgoing packets which were dropped:", row["i/o"]["pack drop out"]), std::cout);
                    PRINT::PrintLine("==================================", std::cout);
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
                LOG::Debug(PRINT::composeMessage("Completed request removed from storage. Id:", boost::uuids::to_string(id)));
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
                    message = PRINT::composeMessage("Failed to parse json response", jsonStr);
                    LOG::Error(message);
                }
                else {
                    message = PRINT::composeMessage("Error message from server", parsedJson["error"]);
                    LOG::Error(message);
                }
            }
            else {
                message = PRINT::composeMessage("Failed to get info from server", static_cast<int>(statusCode), response.getStatusMessage(), response.getBody());
                LOG::Error(message);
            }
            PRINT::PrintLine(message, std::cout);
            auto finishMessage = PRINT::composeMessage("Finished request:", boost::uuids::to_string(id));
            LOG::Info(finishMessage);
            PRINT::PrintLine(finishMessage, std::cout);
        };
    }
}

} // namespace ResourceMonitorClient