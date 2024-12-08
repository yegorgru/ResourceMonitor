from flask import Flask, jsonify
from flask_cors import CORS
import time
import random

app = Flask(__name__)
CORS(app)  # Дозволяє CORS для всіх роутів

@app.route('/basic_info/<int:n>/<int:id>/', methods=['GET'])
def get_data(n, id):

    t = time.localtime()
    current_time = time.strftime("%H:%M:%S", t)
    # Тут можна додати логіку, щоб повертати різні дані в залежності від ID
    data = {
        "machineIp": id,
        "rows":[
            {
                "row": 0,
                "numcpus": random.randint(20, 70),
                "total virt mem GB": random.randint(20, 70),
                "total swap mem GB": random.randint(20, 70),
                "numdisks":  random.randint(20, 70),
                "total_C_disk GB": random.randint(20, 70),
                "timestamp": random.randint(20, 70)
            },
              {
                "row": 0,
                "numcpus": random.randint(20, 70),
                "total virt mem GB": random.randint(20, 70),
                "total swap mem GB": random.randint(20, 70),
                "numdisks":  random.randint(20, 70),
                "total_C_disk GB": random.randint(20, 70),
                "timestamp": random.randint(20, 70)
            },
              {
                "row": 0,
                "numcpus": random.randint(20, 70),
                "total virt mem GB": random.randint(20, 70),
                "total swap mem GB": random.randint(20, 70),
                "numdisks":  random.randint(20, 70),
                "total_C_disk GB": random.randint(20, 70),
                "timestamp": random.randint(20, 70)
            },
              {
                "row": 0,
                "numcpus": random.randint(20, 70),
                "total virt mem GB": random.randint(20, 70),
                "total swap mem GB": random.randint(20, 70),
                "numdisks":  random.randint(20, 70),
                "total_C_disk GB": random.randint(20, 70),
                "timestamp": random.randint(20, 70)
            },
        ],
    }
    return jsonify(data)

@app.route('/cpu/<int:n>/<int:id>/', methods=['GET'])
def get_cpu(n, id):

    t = time.localtime()
    current_time = time.strftime("%H:%M:%S", t)
    # Тут можна додати логіку, щоб повертати різні дані в залежності від ID
    data = {
        "machineIp": id,
        "rows":[
            {
                "row": 0,
                "cpu_times": {
                    "cpu_user": random.randint(20, 70),
                    "cpu_system": random.randint(20, 70),
                    "cpu_idle": random.randint(20, 70),
                },
                "cpu_usage %":  random.randint(20, 70),
                "cpu_freq": {
                    "freq_curr Mhz": random.randint(20, 70),
                    "freq_min Mhz": random.randint(20, 70),
                    "freq_max Mhz": random.randint(20, 70),
                },
                "timestamp": random.randint(20, 70)
            },
              {
                "row": 1,
                "cpu_times": {
                    "cpu_user": random.randint(20, 70),
                    "cpu_system": random.randint(20, 70),
                    "cpu_idle": random.randint(20, 70),
                },
                 "cpu_usage %":  random.randint(20, 70),
                "cpu_freq": {
                    "freq_curr Mhz": random.randint(20, 70),
                    "freq_min Mhz": random.randint(20, 70),
                    "freq_max Mhz": random.randint(20, 70),
                },
                "timestamp": random.randint(20, 70)
            },
              {
                "row": 2,
                "cpu_times": {
                    "cpu_user": random.randint(20, 70),
                    "cpu_system": random.randint(20, 70),
                    "cpu_idle": random.randint(20, 70),
                },
                 "cpu_usage %":  random.randint(20, 70),
                "cpu_freq": {
                    "freq_curr Mhz": random.randint(20, 70),
                    "freq_min Mhz": random.randint(20, 70),
                    "freq_max Mhz": random.randint(20, 70),
                },
                "timestamp": random.randint(20, 70)
            },
              {
                "row": 3,
                "cpu_times": {
                    "cpu_user": random.randint(20, 70),
                    "cpu_system": random.randint(20, 70),
                    "cpu_idle": random.randint(20, 70),
                },
                 "cpu_usage %":  random.randint(20, 70),
                "cpu_freq": {
                    "freq_curr Mhz": random.randint(20, 70),
                    "freq_min Mhz": random.randint(20, 70),
                    "freq_max Mhz": random.randint(20, 70),
                },
                "timestamp": random.randint(20, 70)
            },
        ],
    }
    return jsonify(data)

@app.route('/memory/<int:n>/<int:id>/', methods=['GET'])
def get_memory(n, id):
    t = time.localtime()
    current_time = time.strftime("%H:%M:%S", t)
    # Тут можна додати логіку, щоб повертати різні дані в залежності від ID
    data = {
        "machineIp": id,
        "rows":[
            {
                "row": 0,
                "cpu_times": {
                    "cpu_user": random.randint(20, 70),
                    "cpu_system": random.randint(20, 70),
                    "cpu_idle": random.randint(20, 70),
                },
                "cpu_usage %":  random.randint(20, 70),
                "cpu_freq": {
                    "freq_curr Mhz": random.randint(20, 70),
                    "freq_min Mhz": random.randint(20, 70),
                    "freq_max Mhz": random.randint(20, 70),
                },
                "timestamp": random.randint(20, 70)
            },
        ],
    }
    return jsonify(data)

@app.route('/disks/<int:n>/<int:id>/', methods=['GET'])
def get_disks(n, id):

@app.route('/network/<int:n>/<int:id>/', methods=['GET'])
def get_disks(n, id):

if __name__ == '__main__':
    app.run(debug=True, port=8080)
