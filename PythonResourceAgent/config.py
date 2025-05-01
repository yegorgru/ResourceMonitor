import logging
import socket

LOG_LEVEL = logging.INFO
LOG_FORMAT = '%(filename)s:%(lineno)d [%(levelname)s] %(message)s'

DB_SERVER_URL = "http://localhost:3333"

def get_ip():
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.settimeout(0)
    try:
        # doesn't even have to be reachable
        s.connect(('10.254.254.254', 1))
        IP = s.getsockname()[0]
    except Exception:
        IP = '127.0.0.1'
    finally:
        s.close()
    return IP

MACHINE_IP = get_ip()

ENDPOINTS = {
    "basic_info": f"{DB_SERVER_URL}/basic_info/{MACHINE_IP}",
    "cpu": f"{DB_SERVER_URL}/cpu/{MACHINE_IP}",
    "memory": f"{DB_SERVER_URL}/memory/{MACHINE_IP}",
    "disks": f"{DB_SERVER_URL}/disks/{MACHINE_IP}", 
    "network": f"{DB_SERVER_URL}/network/{MACHINE_IP}"
}

UPDATE_INTERVAL = 30
