import sqlite3
import json
import socket
import threading


server_running = False
server_thread = None


def init_db():
    conn = sqlite3.connect("../Database/machine_states.db")
    cursor = conn.cursor()
    cursor.execute('''
    CREATE TABLE IF NOT EXISTS machine_states (
        name TEXT PRIMARY KEY,
        cpu_usage REAL,
        memory_usage REAL,
        memory_total REAL,
        memory_used REAL,
        disk_usage REAL,
        disk_total REAL,
        disk_used REAL
    )
    ''')
    conn.commit()
    return conn


def save_machine_state(conn, machine_data):
    cursor = conn.cursor()
    cursor.execute('''
    INSERT OR REPLACE INTO machine_states (name, cpu_usage, memory_usage, memory_total, memory_used, disk_usage, disk_total, disk_used)
    VALUES (?, ?, ?, ?, ?, ?, ?, ?)
    ''', (
        machine_data["name"],
        machine_data["cpu"]["usage %"],
        machine_data["memory"]["usage %"],
        machine_data["memory"]["total GB"],
        machine_data["memory"]["used GB"],
        machine_data["disk"]["usage %"],
        machine_data["disk"]["total GB"],
        machine_data["disk"]["used GB"]
    ))
    conn.commit()


def get_machine_state(conn, name):
    cursor = conn.cursor()
    print("name: ", name)
    cursor.execute('SELECT * FROM machine_states WHERE name = ?', (name,))
    row = cursor.fetchone()
    if row:
        print("Return value")
        return {
            "name": row[0],
            "cpu": {"usage %": row[1]},
            "memory": {
                "usage %": row[2],
                "total GB": row[3],
                "used GB": row[4]
            },
            "disk": {
                "usage %": row[5],
                "total GB": row[6],
                "used GB": row[7]
            }
        }
    else:
        print("Return empty")
        return {"error": "not found"}


def send_http_response(client_socket, status_code, response_data):
    headers = {
        "Content-Length": str(len(response_data))
    }

    # Construct the response
    response = f"HTTP/1.1 {status_code}\r\n"
    for header, value in headers.items():
        response += f"{header}: {value}\r\n"
    response += "\r\n"  # End of headers
    response += response_data  # Add the response body (data)

    print(response)

    # Send the full HTTP response
    client_socket.sendall(response.encode('utf-8'))


def run_server(host='localhost', port=10000):
    global server_running
    conn = init_db()

    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server_socket:
        server_socket.bind((host, port))
        server_socket.listen()
        print(f"Server running on {host}:{port}")
        server_running = True

        while server_running:
            client_socket, addr = server_socket.accept()
            with client_socket:
                print(f"Connected by {addr}")
                data = client_socket.recv(4096).decode('utf-8')
                print(data)

                lines = data.splitlines()

                if lines:
                    # The first line of the request contains the HTTP method and path
                    request_line = lines[0]
                    method, path, _ = request_line.split(" ")

                    # Check if the request is a GET or PUT
                    if method == "GET":
                        # Extract the machine name from the path, if included in URL
                        machine_name = path.strip("/")
                        machine_state = get_machine_state(conn, machine_name)

                        # Convert the response to JSON format
                        response_data = json.dumps(machine_state).encode('utf-8')
                        send_http_response(client_socket, "200 OK", response_data.decode('utf-8'))

                    elif method == "PUT":
                        # Extract JSON data from the request body (after headers)
                        try:
                            # Find the blank line separating headers and body
                            blank_line_index = lines.index('')
                            json_data = "\n".join(lines[blank_line_index + 1:])
                            machine_data = json.loads(json_data)

                            # Save machine state to the database
                            save_machine_state(conn, machine_data)
                            send_http_response(client_socket, "200 OK", "Machine state saved successfully")
                        except (json.JSONDecodeError, ValueError) as e:
                            print("Failed to parse JSON data:", e)
                            client_socket.sendall(b"Invalid JSON data.")

                    else:
                        # Send a response for unsupported methods
                        client_socket.sendall(b"HTTP/1.1 405 Method Not Allowed\r\n\r\n")
                else:
                    client_socket.sendall(b"Invalid request format.")

        print("Server stopped.")


def user_input_handler():
    global server_running, server_thread

    while True:
        user_input = input("Enter command (start, stop, exit): ").strip().lower()
        if user_input == "start":
            if not server_running:
                server_thread = threading.Thread(target=run_server)
                server_thread.start()
                print("Server started.")
            else:
                print("Server is already running.")
        elif user_input == "stop":
            if server_running:
                server_running = False
                if server_thread:
                    server_thread.join()
                print("Server stopped.")
            else:
                print("Server is not running.")
        elif user_input == "exit":
            if server_running:
                server_running = False
                if server_thread:
                    server_thread.join()
            print("Exiting application.")
            break
        else:
            print("Unknown command. Please enter 'start', 'stop', or 'exit'.")


if __name__ == "__main__":
    user_input_handler()
