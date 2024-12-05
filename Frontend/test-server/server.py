from flask import Flask, jsonify
from flask_cors import CORS
import time
import random

app = Flask(__name__)
CORS(app)  # Дозволяє CORS для всіх роутів

@app.route('/machines/<int:id>', methods=['GET'])
def get_data(id):

    t = time.localtime()
    current_time = time.strftime("%H:%M:%S", t)
    # Тут можна додати логіку, щоб повертати різні дані в залежності від ID
    data = {
        "time": current_time,
        "id": id,
        "name": f"Machine-{id}",
        "cpu": { "usage %": random.randint(20, 70) },
        "memory": {
            "usage %": random.randint(20, 70) ,
            "total GB": 16,
            "used GB": 10.4,
        },
        "disk": {
            "usage %": random.randint(20, 70) ,
            "total GB": 500,
            "used GB": 275,
        },
    }
    return jsonify(data)

if __name__ == '__main__':
    app.run(debug=True, port=8080)
