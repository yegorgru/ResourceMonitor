#include "HttpCommons.h"
#include "Print.h"
#include "Utils.h"

#include <map>

namespace Http::Commons {

OptionalPrintCallback getPrintCallback(const std::string& resource) {
    using json = nlohmann::json;
    static const std::map<std::string, std::function<void(const nlohmann::json& parsedJson)>> callbacks = {
        {
            "basic_info",
            [](const nlohmann::json& parsedJson) {
                Print::PrintLine(Print::composeMessage("Basic info for", parsedJson["machineIp"]));
                for (const auto& row : parsedJson["rows"]) {
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
            [](const nlohmann::json& parsedJson) {
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
            [](const nlohmann::json& parsedJson) {
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
            [](const nlohmann::json& parsedJson) {
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
            [](const nlohmann::json& parsedJson) {
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
    return found == callbacks.end() ? std::nullopt : OptionalPrintCallback(found->second);
}

} // namespace Http::Commons