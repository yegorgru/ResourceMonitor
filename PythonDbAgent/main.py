import sqlite3
import json
import socket
import threading


server_running = False
server_thread = None
server_socket = None

def init_db():
    conn = sqlite3.connect("../Database/machine_states.db")
    cursor = conn.cursor()

    cursor.execute('''
    CREATE TABLE IF NOT EXISTS IdIp (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        ip TEXT UNIQUE NOT NULL           
    )
    ''')

    cursor.execute('''
    CREATE TABLE IF NOT EXISTS MachineBasic (
        id INTEGER NOT NULL,
        numcpus INTEGER,
        total_virt_mem_gb REAL,
        total_swap_mem_gb REAL,
        numdisks INTEGER,
        total_c_disk_gb REAL,
        Timestamp DATETIME DEFAULT CURRENT_TIMESTAMP NOT NULL,
        PRIMARY KEY (id, Timestamp),
        FOREIGN KEY (id) REFERENCES IdIp (id) ON DELETE CASCADE         
    )
    ''')

    cursor.execute('''
    CREATE TABLE IF NOT EXISTS MachineCPU (
        id INTEGER NOT NULL,
        cpu_user_time REAL,
        cpu_system_time REAL,
        cpu_idle_time REAL,
        cpu_usage REAL,
        freq_curr REAL,
        freq_min REAL,
        freq_max REAL,
        Timestamp DATETIME DEFAULT CURRENT_TIMESTAMP NOT NULL,
        PRIMARY KEY (id, Timestamp),
        FOREIGN KEY (id) REFERENCES IdIp (id) ON DELETE CASCADE           
    )
    ''')

    cursor.execute('''
    CREATE TABLE IF NOT EXISTS MachineMemory (
        id INTEGER NOT NULL,
        virt_usage REAL,
        virt_used REAL,
        virt_free REAL,
        swap_usage REAL,
        swap_used REAL,
        swap_free REAL,
        Timestamp DATETIME DEFAULT CURRENT_TIMESTAMP NOT NULL,
        PRIMARY KEY (id, Timestamp),
        FOREIGN KEY (id) REFERENCES IdIp (id) ON DELETE CASCADE           
    )
    ''')

    cursor.execute('''
    CREATE TABLE IF NOT EXISTS MachineDisk (
        id INTEGER NOT NULL,
        C_usage REAL,
        C_used REAL,
        C_free REAL,
        read_count INTEGER,
        write_count INTEGER,
        read_bytes REAL,
        write_bytes REAL,
        Timestamp DATETIME DEFAULT CURRENT_TIMESTAMP NOT NULL,
        PRIMARY KEY (id, Timestamp),
        FOREIGN KEY (id) REFERENCES IdIp (id) ON DELETE CASCADE           
    )
    ''')

    cursor.execute('''
    CREATE TABLE IF NOT EXISTS MachineNetwork (
        id INTEGER NOT NULL,
        pack_sent INTEGER,
        pack_rcv INTEGER,
        bytes_sent REAL,
        bytes_rcv REAL,
        err_in INTEGER,
        err_out INTEGER,
        drop_in INTEGER,
        drop_out INTEGER,
        connections INTEGER,
        Timestamp DATETIME DEFAULT CURRENT_TIMESTAMP NOT NULL,
        PRIMARY KEY (id, Timestamp),
        FOREIGN KEY (id) REFERENCES IdIp (id) ON DELETE CASCADE           
    )
    ''')
    conn.commit()
    return conn


def save_machine_state(conn, machine_data, path):
    cursor = conn.cursor()
    table, ip = path.split('/')
    cursor.execute('SELECT * FROM IdIp WHERE ip = ?', (ip,))
    results = cursor.fetchone()
    if not(results):
        cursor.execute('INSERT INTO IdIp (ip) VALUES (?)', (ip,))
        conn.commit()
        cursor.execute('SELECT * FROM IdIp WHERE ip = ?', (ip,))
        results = cursor.fetchone()
    machine_id = results[0]
    if table == 'basic_info':
        cursor.execute('''
            INSERT INTO MachineBasic (id, numcpus, total_virt_mem_gb, total_swap_mem_gb, numdisks, total_c_disk_gb)
            VALUES (?, ?, ?, ?, ?, ?)
            ''', (
            machine_id,
            machine_data["numcpus"],
            machine_data["total virt mem GB"],
            machine_data["total swap mem GB"],
            machine_data["numdisks"],
            machine_data["total_C_disk GB"]
        ))
    elif table == 'cpu':
        cursor.execute('''
            INSERT INTO MachineCPU (id, cpu_user_time, cpu_system_time, cpu_idle_time, cpu_usage, freq_curr, freq_min, freq_max)
            VALUES (?, ?, ?, ?, ?, ?, ?, ?)
            ''', (
            machine_id,
            machine_data["cpu_times"]["cpu_user"],
            machine_data["cpu_times"]["cpu_system"],
            machine_data["cpu_times"]["cpu_idle"],
            machine_data["cpu_usage %"],
            machine_data["cpu_freq"]["freq_curr Mhz"],
            machine_data["cpu_freq"]["freq_min Mhz"],
            machine_data["cpu_freq"]["freq_max Mhz"]
        ))
    elif table == 'memory':
        cursor.execute('''
            INSERT INTO MachineMemory (id, virt_usage, virt_used, virt_free, swap_usage, swap_used, swap_free)
            VALUES (?, ?, ?, ?, ?, ?, ?)
            ''', (
            machine_id,
            machine_data["virt_memory"]["usage virt %"],
            machine_data["virt_memory"]["used virt GB"],
            machine_data["virt_memory"]["available virt GB"],
            machine_data["swap_memory"]["usage swap %"],
            machine_data["swap_memory"]["used swap GB"],
            machine_data["swap_memory"]["free swap GB"]
        ))
    elif table == 'disks':
        cursor.execute('''
            INSERT INTO MachineDisk (id, C_usage, C_used, C_free, read_count, write_count, read_bytes, write_bytes)
            VALUES (?, ?, ?, ?, ?, ?, ?, ?)
            ''', (
            machine_id,
            machine_data["disk usage"]["usage %"],
            machine_data["disk usage"]["used GB"],
            machine_data["disk usage"]["free GB"],
            machine_data["disk i/o"]["read_count"],
            machine_data["disk i/o"]["write_count"],
            machine_data["disk i/o"]["read_bytes GB"],
            machine_data["disk i/o"]["write_bytes GB"]
        ))
    elif table == 'network':
        cursor.execute('''
            INSERT INTO MachineNetwork (id, pack_sent, pack_rcv, bytes_sent, bytes_rcv, err_in, err_out, drop_in, drop_out, connections)
            VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
            ''', (
            machine_id,
            machine_data["i/o"]["packets_sent"],
            machine_data["i/o"]["packets_recv"],
            machine_data["i/o"]["bytes_sent"],
            machine_data["i/o"]["bytes_recv"],
            machine_data["i/o"]["errors in"],
            machine_data["i/o"]["errors out"],
            machine_data["i/o"]["pack drop in"],
            machine_data["i/o"]["pack drop out"],
            machine_data["connections"]
        ))
    else:
        raise Exception("Invalid request path!")
        return -1
    conn.commit()


def get_machine_state(conn, path):
    cursor = conn.cursor()
    table, numrecs, ip = path.split('/')
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
        }, True
    else:
        print("Return empty")
        return {"error": "not found"}, False


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
    global server_running, server_socket
    conn = init_db()

    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
        server_socket = sock
        sock.bind((host, port))
        sock.listen()
        print(f"Server running on {host}:{port}")
        server_running = True

        while server_running:
            try:
                sock.settimeout(1)
                client_socket, addr = sock.accept()
                with client_socket:
                    print(f"Connected by {addr}")
                    data = client_socket.recv(4096).decode('utf-8')
                    print(data)

                    lines = data.splitlines()

                    if lines:
                        # The first line of the request contains the HTTP method and path
                        request_line = lines[0]
                        method, path, _ = request_line.split(" ")
                        method = 'GET'   # JUST TESTING

                        # Check if the request is a GET or PUT
                        if method == "GET":
                            # Extract the machine name from the path, if included in URL
                            path = '/basic_info/3/<machine ip>' # JUST TESTING
                            path = path.strip("/")
                            machine_state, success = get_machine_state(conn, path)

                            if success:
                                response_data = json.dumps(machine_state)
                                send_http_response(client_socket, "200 OK", response_data)
                            else:
                                response_data = "Error - machine not found"
                                send_http_response(client_socket, "500 Server Error", response_data)
                        elif method == "PUT":
                            # Extract JSON data from the request body (after headers)
                            try:
                                # Find the blank line separating headers and body
                                blank_line_index = lines.index('')
                                json_data = "\n".join(lines[blank_line_index + 1:])
                                machine_data = json.loads(json_data)
                                path = path.strip("/")

                                # Save machine state to the database
                                save_machine_state(conn, machine_data, path)
                                send_http_response(client_socket, "200 OK", "Machine state saved successfully")
                            except (json.JSONDecodeError, ValueError) as e:
                                print("Failed to parse JSON data:", e)
                                send_http_response(client_socket, "500 Server Error", "Failed to parse JSON data")
                        else:
                            send_http_response(client_socket, "501 Not Implemented", "")
                    else:
                        send_http_response(client_socket, "500 Server Error", "Invalid request format")
            except socket.timeout:
                continue
            except OSError as e:
                if not server_running:
                    print("Server stopped listening.")
                else:
                    print(f"Socket error: {e}")
            except Exception as e:
                print(f"Error: {e}")


def user_input_handler():
    global server_running, server_thread, server_socket

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
                if server_socket:
                    try:
                        server_socket.close()
                        server_socket = None
                    except OSError as e:
                        print(f"Error closing socket: {e}")
                if server_thread:
                    server_thread.join()
                print("Server stopped.")
            else:
                print("Server is not running.")
        elif user_input == "exit":
            if server_running:
                server_running = False
                if server_socket:
                    try:
                        server_socket.close()
                        server_socket = None
                    except OSError as e:
                        print(f"Error closing socket: {e}")
                if server_thread:
                    server_thread.join()
            print("Exiting application.")
            break
        else:
            print("Unknown command. Please enter 'start', 'stop', or 'exit'.")


if __name__ == "__main__":
    user_input_handler()
