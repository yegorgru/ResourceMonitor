import psutil
import requests
import socket
import json
import time
import threading

machine_ip = socket.gethostbyname(socket.gethostname())
urlBasic = f"http://localhost:10000/basic_info/{machine_ip}"
urlCPU = f"http://localhost:10000/cpu/{machine_ip}"
urlMemory = f"http://localhost:10000/memory/{machine_ip}"
urlDisks = f"http://localhost:10000/disks/{machine_ip}"
urlNetwork = f"http://localhost:10000/network/{machine_ip}"


send_requests = False
running = True


def get_basic_stats():
    numcpus = psutil.cpu_count()
    virtual_memory = psutil.virtual_memory()
    disk_usage = psutil.disk_usage('/')

    return {
        "ip": machine_ip,
        "numcpus": numcpus,
        "total virt mem": virtual_memory.total / (1024 ** 3),
        "total disk": disk_usage.total / (1024 ** 3)
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
            "freq_curr": cpu_freq.current,
            "freq_min": cpu_freq.min,
            "freq_max": cpu_freq.max
        }
    }

def get_memory_stats():
    virtual_memory = psutil.virtual_memory()


    return {
        "ip": machine_ip,
        "virt_memory": {
            "usage %": virtual_memory.percent,
            "used GB": virtual_memory.used / (1024 ** 3),
            "available GB": virtual_memory.available / (1024 ** 3),
        },
        "cpu_usage %": cpu_percent,
        "cpu_freq": {
            "freq_curr": cpu_freq.current,
            "freq_min": cpu_freq.min,
            "freq_max": cpu_freq.max
        }
    }


def get_resources_stats():
    cpu_percent = psutil.cpu_percent(interval=1)
    virtual_memory = psutil.virtual_memory()
    disk_usage = psutil.disk_usage('/')

    return {
        "ip": machine_ip,
        "cpu": {"usage %": cpu_percent},
        "memory": {
            "usage %": virtual_memory.percent,
            "used GB": virtual_memory.used / (1024 ** 3)
        },
        "disk": {
            "usage %": disk_usage.percent,
            "used GB": disk_usage.used / (1024 ** 3)
        }
    }


def get_network_stats():
    network_io = psutil.net_io_counters()
    connections = len(psutil.net_connections())

    return {
        "ip": machine_ip,
        "i/o": {
            "packets_sent": network_io.packets_sent,
            "packets_recv": network_io.packets_recv
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
                    resourcesStats = get_resources_stats()
                    responseResources = requests.put(urlResources, data=json.dumps(resourcesStats),
                                                    headers={'Content-Type': 'application/json'})
                    if responseResources.status_code == 200:
                        print("Resources info sent successfully:", resourcesStats)
                    else:
                        print("Failed to send resources info:", responseResources.status_code, responseResources.text)

                    networkStats = get_network_stats()
                    responseNetwork = requests.put(urlNetwork, data=json.dumps(networkStats),
                                                   headers={'Content-Type': 'application/json'})
                    if responseNetwork.status_code == 200:
                        print("Network info sent successfully:", networkStats)
                    else:
                        print("Failed to send network info:", responseNetwork.status_code, responseNetwork.text)
            except Exception as e:
                print("Error:", e)
            time.sleep(30)


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
