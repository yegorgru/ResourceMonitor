import psutil
import requests
import socket
import json
import time
import threading

machine_ip = socket.gethostbyname(socket.gethostname())
urlBasic = f"http://localhost:8080/basic_info/{machine_ip}"
urlResource = f"http://localhost:8080/resource/{machine_ip}"
urlNetwork = f"http://localhost:8080/networking/{machine_ip}"


send_requests = False
running = True


def get_basic_stats():
    cpu_percent = psutil.cpu_percent(interval=1)
    virtual_memory = psutil.virtual_memory()
    disk_usage = psutil.disk_usage('/')

    return {
        "name": machine_ip,
        "cpu": {"usage %": cpu_percent},
        "memory": {
            "usage %": virtual_memory.percent,
            "total GB": virtual_memory.total / (1024 ** 3),
            "used GB": virtual_memory.used / (1024 ** 3)
        },
        "disk": {
            "usage %": disk_usage.percent,
            "total GB": disk_usage.total / (1024 ** 3),
            "used GB": disk_usage.used / (1024 ** 3)
        }
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
                    basicStats = get_basic_stats()
                    responseResource = requests.put(urlResource, data=json.dumps(basicStats),
                                                    headers={'Content-Type': 'application/json'})
                    if responseResource.status_code == 200:
                        print("Resource info sent successfully:", basicStats)
                    else:
                        print("Failed to send resource info:", responseResource.status_code, responseResource.text)

                    basicStats = get_basic_stats()
                    responseNetwork = requests.put(urlNetwork, data=json.dumps(basicStats),
                                                   headers={'Content-Type': 'application/json'})
                    if responseNetwork.status_code == 200:
                        print("Network info sent successfully:", basicStats)
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
