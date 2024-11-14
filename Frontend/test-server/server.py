from flask import Flask, jsonify
from flask_cors import CORS
import time

app = Flask(__name__)
CORS(app)  # Дозволяє CORS для всіх роутів

@app.route('/api/data/<int:id>', methods=['GET'])
def get_data(id):

    t = time.localtime()
    current_time = time.strftime("%H:%M:%S", t)
    # Тут можна додати логіку, щоб повертати різні дані в залежності від ID
    data = {
        "time": current_time,
        "id": id,
        "name": f"Computer-{id}",
        "cpu": { "usage %": 35 },
        "memory": {
            "usage %": 65,
            "total GB": 16,
            "used GB": 10.4,
        },
        "disk": {
            "usage %": 55,
            "total GB": 500,
            "used GB": 275,
        },
    }
    return jsonify(data)

if __name__ == '__main__':
    app.run(debug=True)
