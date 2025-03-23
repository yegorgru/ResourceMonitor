import socket
import logging

LOG_LEVEL = logging.INFO
LOG_FORMAT = '%(filename)s:%(lineno)d [%(levelname)s] %(message)s'

MACHINE_IP = socket.gethostbyname(socket.gethostname())
DB_SERVER_URL = "http://localhost:3333"

ENDPOINTS = {
    "basic_info": f"{DB_SERVER_URL}/basic_info/{MACHINE_IP}",
    "cpu": f"{DB_SERVER_URL}/cpu/{MACHINE_IP}",
    "memory": f"{DB_SERVER_URL}/memory/{MACHINE_IP}",
    "disks": f"{DB_SERVER_URL}/disks/{MACHINE_IP}", 
    "network": f"{DB_SERVER_URL}/network/{MACHINE_IP}"
}

UPDATE_INTERVAL = 30
