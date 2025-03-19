import psutil
import requests
import socket
import json
import threading

machine_ip = socket.gethostbyname(socket.gethostname())
urlBasic = f"http://localhost:3333/basic_info/{machine_ip}"
urlCPU = f"http://localhost:3333/cpu/{machine_ip}"
urlMemory = f"http://localhost:3333/memory/{machine_ip}"
urlDisks = f"http://localhost:3333/disks/{machine_ip}"
urlNetwork = f"http://localhost:3333/network/{machine_ip}"


send_requests = False
running = True

stop_event = threading.Event()


def get_basic_stats():
    num_cpus = psutil.cpu_count()
    virtual_memory = psutil.virtual_memory()
    swap_memory = psutil.swap_memory()
    num_disks = len(psutil.disk_partitions())
    disk_c_usage = psutil.disk_usage('/')

    return {
        "ip": machine_ip,
        "numcpus": num_cpus,
        "total virt mem GB": virtual_memory.total / (1024 ** 3),
        "total swap mem GB": swap_memory.total / (1024 ** 3),
        "numdisks": num_disks,
        "total_C_disk GB": disk_c_usage.total / (1024 ** 3)
    }

def get_cpu_stats():
    cpu_times = psutil.cpu_times()
    cpu_percent = psutil.cpu_percent(interval=1)
    cpu_freq = psutil.cpu_freq()


    return {
        "ip": machine_ip,
        "cpu_times": {
            "cpu_user": cpu_times.user,
            "cpu_system": cpu_times.system,
            "cpu_idle": cpu_times.idle
        },
        "cpu_usage %": cpu_percent,
        "cpu_freq": {
            "freq_curr Mhz": cpu_freq.current,
            "freq_min Mhz": cpu_freq.min,
            "freq_max Mhz": cpu_freq.max
        }
    }

def get_memory_stats():
    virtual_memory = psutil.virtual_memory()
    swap_memory = psutil.swap_memory()


    return {
        "ip": machine_ip,
        "virt_memory": {
            "usage virt %": virtual_memory.percent,
            "used virt GB": virtual_memory.used / (1024 ** 3),
            "available virt GB": virtual_memory.available / (1024 ** 3)
        },
        "swap_memory": {
            "usage swap %": swap_memory.percent,
            "used swap GB": swap_memory.used / (1024 ** 3),
            "free swap GB": swap_memory.free / (1024 ** 3)
        }
    }


def get_disk_stats():
    disk_C_usage = psutil.disk_usage('/')
    disk_io = psutil.disk_io_counters()

    return {
        "ip": machine_ip,
        "disk usage": {
            "usage %": disk_C_usage.percent,
            "used GB": disk_C_usage.used / (1024 ** 3),
            "free GB": disk_C_usage.free / (1024 ** 3)
        },
        "disk i/o": {
            "read_count": disk_io.read_count,
            "write_count": disk_io.write_count,
            "read_bytes GB": disk_io.read_bytes / (1024 ** 3),
            "write_bytes GB": disk_io.write_bytes / (1024 ** 3)
        }
    }


def get_network_stats():
    network_io = psutil.net_io_counters()
    connections = len(psutil.net_connections())

    return {
        "ip": machine_ip,
        "i/o": {
            "packets_sent": network_io.packets_sent,
            "packets_recv": network_io.packets_recv,
            "bytes_sent": network_io.bytes_sent,
            "bytes_recv": network_io.packets_recv,
            "errors in": network_io.errin,
            "errors out": network_io.errout,
            "pack drop in": network_io.dropin,
            "pack drop out": network_io.dropout
        },
        "connections": connections
    }


def send_stats():
    global send_requests, running
    firstReq = True
    while running:
        if send_requests:
            try:
                if firstReq:
                    basicStats = get_basic_stats()
                    responseBasic = requests.put(urlBasic, data=json.dumps(basicStats),
                                                 headers={'Content-Type': 'application/json'})
                    if responseBasic.status_code == 200:
                        print("Basic info sent successfully:", basicStats)
                        firstReq = False
                    else:
                        print("Failed to send basic info:", responseBasic.status_code, responseBasic.text)
                else:
                    cpuStats = get_cpu_stats()
                    responseCPU = requests.put(urlCPU, data=json.dumps(cpuStats),
                                                    headers={'Content-Type': 'application/json'})
                    if responseCPU.status_code == 200:
                        print("CPU info sent successfully:", cpuStats)
                    else:
                        print("Failed to send CPU info:", responseCPU.status_code, responseCPU.text)

                    memoryStats = get_memory_stats()
                    responseMemory = requests.put(urlMemory, data=json.dumps(memoryStats),
                                                   headers={'Content-Type': 'application/json'})
                    if responseMemory.status_code == 200:
                        print("Memory info sent successfully:", memoryStats)
                    else:
                        print("Failed to send memory info:", responseMemory.status_code, responseMemory.text)

                    disksStats = get_disk_stats()
                    responseDisks = requests.put(urlDisks, data=json.dumps(disksStats),
                                                  headers={'Content-Type': 'application/json'})
                    if responseDisks.status_code == 200:
                        print("Disks info sent successfully:", disksStats)
                    else:
                        print("Failed to send disks info:", responseDisks.status_code, responseDisks.text)

                    networkStats = get_network_stats()
                    responseNetwork = requests.put(urlNetwork, data=json.dumps(networkStats),
                                                 headers={'Content-Type': 'application/json'})
                    if responseNetwork.status_code == 200:
                        print("Network info sent successfully:", networkStats)
                    else:
                        print("Failed to send network info:", responseNetwork.status_code, responseNetwork.text)
            except Exception as e:
                print("Error:", e)
            if stop_event.wait(30):
                break


def user_input_handler():
    global send_requests, running
    while True:
        user_input = input("Enter command (start, stop, exit): ").strip().lower()
        if user_input == "start":
            send_requests = True
            print("Started sending requests.")
        elif user_input == "stop":
            send_requests = False
            print("Stopped sending requests.")
        elif user_input == "exit":
            send_requests = False
            running = False
            stop_event.set()
            print("Exiting application.")
            break
        else:
            print("Unknown command. Please enter 'start', 'stop', or 'exit'.")


data_thread = threading.Thread(target=send_stats)
input_thread = threading.Thread(target=user_input_handler)

data_thread.start()
input_thread.start()

input_thread.join()
data_thread.join()
print("Application terminated.")
