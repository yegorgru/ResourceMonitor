#include "Client.h"
#include "Log.h"

#include <boost/uuid/uuid_io.hpp>
#include <nlohmann/json.hpp>

namespace Http::Asio {

Client::Client()
    : mWork(boost::asio::make_work_guard(mIoService))
{
    mThreadIo = std::jthread([this]() { 
        mIoService.run();
    });
}

Client::~Client()
{
    Log::Debug("Destroying client");
    if (mWork.owns_work()) {
        close();
    }
}

Client::OptionalRequestId Client::makeRequest(int serverPort, const std::string& serverName, const std::string& resource, const std::string& count, const std::string& ipAddress) {
    Log::Debug("Making request");
    auto callback = getCallback(resource);
    if (!callback) {
        return std::nullopt;
    }
    auto request = std::make_shared<Request>(mIoService, serverName, serverPort, *callback);
    std::string endpoint = resource + "/" + count + "/" + ipAddress;
    request->get(endpoint);
    const auto& id = request->getId();
    {
        std::lock_guard<std::mutex> lg(mRequestsMutex);
        mRequests[id] = request;
    }
    return boost::uuids::to_string(id);
}

void Client::cancelRequest(const std::string strId) {
    Log::Debug("Canceling request");
    static boost::uuids::string_generator stringGen;
    std::string message;
    try
    {
        Request::Id id = stringGen(strId);
        std::lock_guard<std::mutex> lg(mRequestsMutex);
        auto found = mRequests.find(id);
        if (found != mRequests.end()) {
            if (!found->second->isCompleted()) {
                found->second->cancel();
                message = Print::composeMessage("Request canceled. Id:", strId);
                Log::Info(message);
                mRequests.erase(found);
                Log::Debug(Print::composeMessage("Canceled request removed from storage. Id:", boost::uuids::to_string(id)));
            }
            else {
                message = Print::composeMessage("Error: completed request still in storage. Id:", strId);
                Log::Error(message);
            }
        }
        else {
            message = Print::composeMessage("No requests with such id in storage. Id:", strId);
            Log::Info(message);
        }
    }
    catch (const std::runtime_error&)
    {
        message = Print::composeMessage("Failed to convert string to valid id. Id:", strId);
        Log::Error(message);
    }
    Print::PrintLine(message);
}

void Client::close() {
    Log::Debug("Client close");
    mWork.reset();
    mThreadIo.join();
}

Client::OptionalCallback Client::getCallback(const std::string& resource) {
    using json = nlohmann::json;
    static const std::map<std::string, std::function<void(const nlohmann::json& parsedJson)>> callbacks = {
        {
            "basic_info",
            [this](const nlohmann::json& parsedJson) {
                Print::PrintLine(Print::composeMessage("Basic info for", parsedJson["machineIp"]));
                for (const auto& row : parsedJson["rows"]) {
                    Log::Debug(Print::composeMessage(parsedJson.dump()));
                    Print::PrintLine("==================================");
                    Print::PrintLine(Print::composeMessage("Timestamp:", row["timestamp"]));
                    Print::PrintLine(Print::composeMessage("Cpu count:", row["numcpus"]));
                    Print::PrintLine(Print::composeMessage("Total virtual memory (GB):", row["total virt mem GB"]));
                    Print::PrintLine(Print::composeMessage("Total swap memory (GB):", row["total swap mem GB"]));
                    Print::PrintLine(Print::composeMessage("Disk partitions count:", row["numdisks"]));
                    Print::PrintLine(Print::composeMessage("Total disk (GB):", row["total_C_disk GB"]));
                    Print::PrintLine("==================================");
                }
            }
        },
        {
            "cpu",
            [this](const nlohmann::json& parsedJson) {
                Print::PrintLine(Print::composeMessage("Cpu info for", parsedJson["machineIp"]));
                for (const auto& row : parsedJson["rows"]) {
                    Print::PrintLine("==================================");
                    Print::PrintLine(Print::composeMessage("Timestamp:", row["timestamp"]));
                    Print::PrintLine(Print::composeMessage("Cpu usage (%):", row["cpu_usage %"]));
                    Print::PrintLine(Print::composeMessage("Time spent by normal processes executing in user mode:", row["cpu_times"]["cpu_user"]));
                    Print::PrintLine(Print::composeMessage("Time spent by processes executing in kernel mode:", row["cpu_times"]["cpu_system"]));
                    Print::PrintLine(Print::composeMessage("Idle time:", row["cpu_times"]["cpu_idle"]));
                    Print::PrintLine(Print::composeMessage("Current cpu frequency Mhz:", row["cpu_freq"]["freq_curr Mhz"]));
                    Print::PrintLine("==================================");
                }
            }
        },
        {
            "memory",
            [this](const nlohmann::json& parsedJson) {
                Print::PrintLine(Print::composeMessage("Memory info for", parsedJson["machineIp"]));
                for (const auto& row : parsedJson["rows"]) {
                    Print::PrintLine("==================================");
                    Print::PrintLine(Print::composeMessage("Timestamp:", row["timestamp"]));
                    Print::PrintLine(Print::composeMessage("Virtual memory usage (%):", row["virt_memory"]["usage virt %"]));
                    Print::PrintLine(Print::composeMessage("Used virtual memory (GB):", row["virt_memory"]["used virt GB"]));
                    Print::PrintLine(Print::composeMessage("Available virtual memory (GB):", row["virt_memory"]["available virt GB"]));
                    Print::PrintLine(Print::composeMessage("Swap memory usage (%):", row["swap_memory"]["usage swap %"]));
                    Print::PrintLine(Print::composeMessage("Used swap memory (GB):", row["swap_memory"]["used swap GB"]));
                    Print::PrintLine(Print::composeMessage("Free swap memory (GB):", row["virt_memory"]["available virt GB"]));
                    Print::PrintLine("==================================");
                }
            }
        },
        {
            "disks",
            [this](const nlohmann::json& parsedJson) {
                Print::PrintLine(Print::composeMessage("Disks info for", parsedJson["machineIp"]));
                for (const auto& row : parsedJson["rows"]) {
                    Print::PrintLine("==================================");
                    Print::PrintLine(Print::composeMessage("Timestamp:", row["timestamp"]));
                    Print::PrintLine(Print::composeMessage("Disk usage (%):", row["disk usage"]["usage %"]));
                    Print::PrintLine(Print::composeMessage("Used disk (GB):", row["disk usage"]["used GB"]));
                    Print::PrintLine(Print::composeMessage("Free disk (GB):", row["disk usage"]["free GB"]));
                    Print::PrintLine(Print::composeMessage("Disk i/o read count:", row["disk i/o"]["read_count"]));
                    Print::PrintLine(Print::composeMessage("Disk i/o write count:", row["disk i/o"]["read_count"]));
                    Print::PrintLine(Print::composeMessage("Number of GB read:", row["disk i/o"]["read_bytes GB"]));
                    Print::PrintLine(Print::composeMessage("Number of GB written:", row["disk i/o"]["write_bytes GB"]));
                    Print::PrintLine("==================================");
                }
            }
        },
        {
            "network",
            [this](const nlohmann::json& parsedJson) {
                Print::PrintLine(Print::composeMessage("Network info for", parsedJson["machineIp"]));
                for (const auto& row : parsedJson["rows"]) {
                    Print::PrintLine("==================================");
                    Print::PrintLine(Print::composeMessage("Timestamp:", row["timestamp"]));
                    Print::PrintLine(Print::composeMessage("Number of connections:", row["connections"]));
                    Print::PrintLine(Print::composeMessage("Number of packets sent:", row["i/o"]["packets_sent"]));
                    Print::PrintLine(Print::composeMessage("Number of packets received:", row["i/o"]["packets_recv"]));
                    Print::PrintLine(Print::composeMessage("Number of bytes sent:", row["i/o"]["bytes_sent"]));
                    Print::PrintLine(Print::composeMessage("Number of bytes received:", row["i/o"]["bytes_recv"]));
                    Print::PrintLine(Print::composeMessage("Total number of errors while receiving:", row["i/o"]["errors in"]));
                    Print::PrintLine(Print::composeMessage("Total number of errors while sending:", row["i/o"]["errors out"]));
                    Print::PrintLine(Print::composeMessage("Total number of incoming packets which were dropped:", row["i/o"]["pack drop in"]));
                    Print::PrintLine(Print::composeMessage("Total number of outgoing packets which were dropped:", row["i/o"]["pack drop out"]));
                    Print::PrintLine("==================================");
                }
            }
        },
    };
    auto found = callbacks.find(resource);
    if (found == callbacks.end()) {
        return std::nullopt;
    }
    else {
        return [this, found](const MessageResponse& response, const Request::Id& id) {
            auto statusCode = response.getStatusCode();
            if (statusCode == StatusCode::ClientClosedRequest) {
                Log::Debug("Callback for canceled request");
                return;
            }
            else {
                std::lock_guard<std::mutex> lg(mRequestsMutex);
                mRequests.erase(id);
                Log::Debug(Print::composeMessage("Completed request removed from storage. Id:", boost::uuids::to_string(id)));
            }
            std::string message;
            if (statusCode == StatusCode::Ok) {
                const auto& jsonStr = response.getBody();
                auto parsedJson = json::parse(jsonStr);
                if (!parsedJson.empty() && !parsedJson.contains("error")) {

                    found->second(parsedJson);

                    message = "Request processed successfully";
                    Log::Info(message);
                }
                else if (parsedJson.empty()) {
                    message = Print::composeMessage("Failed to parse json response", jsonStr);
                    Log::Error(message);
                }
                else {
                    message = Print::composeMessage("Error message from server", parsedJson["error"]);
                    Log::Error(message);
                }
            }
            else {
                message = Print::composeMessage("Failed to get info from server", static_cast<int>(statusCode), response.getStatusMessage(), response.getBody());
                Log::Error(message);
            }
            Print::PrintLine(message);
            auto finishMessage = Print::composeMessage("Finished request:", boost::uuids::to_string(id));
            Log::Info(finishMessage);
            Print::PrintLine(finishMessage);
        };
    }
}

} // namespace Http::Asio