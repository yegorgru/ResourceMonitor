import sqlite3
from config import DB_PATH
import logging
import threading

logger = logging.getLogger(__name__)

class DatabaseManager:
    def __init__(self):
        self._local = threading.local()
        self._init_db()  # Initialize schema in main thread
    
    def _get_connection(self):
        if not hasattr(self._local, 'conn'):
            self._local.conn = sqlite3.connect(DB_PATH)
        return self._local.conn

    def _init_db(self):
        conn = sqlite3.connect(DB_PATH)
        cursor = conn.cursor()

        tables = {
            'IdIp': '''(
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                ip TEXT UNIQUE NOT NULL           
            )''',
            'MachineBasic': '''(
                id INTEGER NOT NULL,
                numcpus INTEGER,
                total_virt_mem_gb REAL,
                total_swap_mem_gb REAL,
                numdisks INTEGER,
                total_c_disk_gb REAL,
                Timestamp DATETIME DEFAULT CURRENT_TIMESTAMP NOT NULL,
                PRIMARY KEY (id, Timestamp),
                FOREIGN KEY (id) REFERENCES IdIp (id) ON DELETE CASCADE         
            )''',
            'MachineCPU': '''(
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
            )''',
            'MachineMemory': '''(
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
            )''',
            'MachineDisk': '''(
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
            )''',
            'MachineNetwork': '''(
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
            )'''
        }

        for table_name, schema in tables.items():
            cursor.execute(f'CREATE TABLE IF NOT EXISTS {table_name} {schema}')

        conn.commit()
        conn.close()

    def _save_basic_info(self, cursor, machine_id, data):
        cursor.execute('''
            INSERT INTO MachineBasic (id, numcpus, total_virt_mem_gb, total_swap_mem_gb, numdisks, total_c_disk_gb)
            VALUES (?, ?, ?, ?, ?, ?)
        ''', (
            machine_id,
            data["numcpus"],
            data["total virt mem GB"],
            data["total swap mem GB"],
            data["numdisks"],
            data["total_C_disk GB"]
        ))

    def _save_cpu_info(self, cursor, machine_id, data):
        cursor.execute('''
            INSERT INTO MachineCPU (id, cpu_user_time, cpu_system_time, cpu_idle_time, cpu_usage, freq_curr, freq_min, freq_max)
            VALUES (?, ?, ?, ?, ?, ?, ?, ?)
        ''', (
            machine_id,
            data["cpu_times"]["cpu_user"],
            data["cpu_times"]["cpu_system"],
            data["cpu_times"]["cpu_idle"],
            data["cpu_usage %"],
            data["cpu_freq"]["freq_curr Mhz"],
            data["cpu_freq"]["freq_min Mhz"],
            data["cpu_freq"]["freq_max Mhz"]
        ))

    def _save_memory_info(self, cursor, machine_id, data):
        cursor.execute('''
            INSERT INTO MachineMemory (id, virt_usage, virt_used, virt_free, swap_usage, swap_used, swap_free)
            VALUES (?, ?, ?, ?, ?, ?, ?)
        ''', (
            machine_id,
            data["virt_memory"]["usage virt %"],
            data["virt_memory"]["used virt GB"],
            data["virt_memory"]["available virt GB"],
            data["swap_memory"]["usage swap %"],
            data["swap_memory"]["used swap GB"],
            data["swap_memory"]["free swap GB"]
        ))

    def _save_disk_info(self, cursor, machine_id, data):
        cursor.execute('''
            INSERT INTO MachineDisk (id, C_usage, C_used, C_free, read_count, write_count, read_bytes, write_bytes)
            VALUES (?, ?, ?, ?, ?, ?, ?, ?)
        ''', (
            machine_id,
            data["disk usage"]["usage %"],
            data["disk usage"]["used GB"],
            data["disk usage"]["free GB"],
            data["disk i/o"]["read_count"],
            data["disk i/o"]["write_count"],
            data["disk i/o"]["read_bytes GB"],
            data["disk i/o"]["write_bytes GB"]
        ))

    def _save_network_info(self, cursor, machine_id, data):
        cursor.execute('''
            INSERT INTO MachineNetwork (id, pack_sent, pack_rcv, bytes_sent, bytes_rcv, err_in, err_out, drop_in, drop_out, connections)
            VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
        ''', (
            machine_id,
            data["i/o"]["packets_sent"],
            data["i/o"]["packets_recv"],
            data["i/o"]["bytes_sent"],
            data["i/o"]["bytes_recv"],
            data["i/o"]["errors in"],
            data["i/o"]["errors out"],
            data["i/o"]["pack drop in"],
            data["i/o"]["pack drop out"],
            data["connections"]
        ))

    def save_machine_state(self, machine_data, path):
        conn = self._get_connection()
        cursor = conn.cursor()
        table, ip = path.split('/')
        
        try:
            cursor.execute('SELECT * FROM IdIp WHERE ip = ?', (ip,))
            results = cursor.fetchone()
            
            if not results:
                cursor.execute('INSERT INTO IdIp (ip) VALUES (?)', (ip,))
                conn.commit()
                cursor.execute('SELECT * FROM IdIp WHERE ip = ?', (ip,))
                results = cursor.fetchone()
                
            machine_id = results[0]
            
            save_methods = {
                'basic_info': self._save_basic_info,
                'cpu': self._save_cpu_info,
                'memory': self._save_memory_info,
                'disks': self._save_disk_info,
                'network': self._save_network_info
            }
            
            if table in save_methods:
                save_methods[table](cursor, machine_id, machine_data)
                conn.commit()
                return True
            else:
                raise ValueError(f"Unknown table type: {table}")
        except Exception as e:
            logger.error(f"Database error: {e}")
            return False

    def _get_basic_info(self, cursor, machine_id, limit):
        cursor.execute('''
            SELECT * FROM MachineBasic 
            WHERE id = ? ORDER BY Timestamp DESC LIMIT ?
        ''', (machine_id, limit))
        rows = cursor.fetchall()
        return [{
            "row": i + 1,
            "numcpus": row[1],
            "total virt mem GB": row[2],
            "total swap mem GB": row[3],
            "numdisks": row[4],
            "total_C_disk GB": row[5],
            "timestamp": row[6]
        } for i, row in enumerate(rows)]

    def _get_cpu_info(self, cursor, machine_id, limit):
        cursor.execute('''
            SELECT * FROM MachineCPU 
            WHERE id = ? ORDER BY Timestamp DESC LIMIT ?
        ''', (machine_id, limit))
        rows = cursor.fetchall()
        return [{
            "row": i + 1,
            "cpu_times": {
                "cpu_user": row[1],
                "cpu_system": row[2],
                "cpu_idle": row[3]
            },
            "cpu_usage %": row[4],
            "cpu_freq": {
                "freq_curr Mhz": row[5],
                "freq_min Mhz": row[6],
                "freq_max Mhz": row[7]
            },
            "timestamp": row[8]
        } for i, row in enumerate(rows)]

    def _get_memory_info(self, cursor, machine_id, limit):
        cursor.execute('''
            SELECT * FROM MachineMemory 
            WHERE id = ? ORDER BY Timestamp DESC LIMIT ?
        ''', (machine_id, limit))
        rows = cursor.fetchall()
        return [{
            "row": i + 1,
            "virt_memory": {
                "usage virt %": row[1],
                "used virt GB": row[2],
                "available virt GB": row[3]
            },
            "swap_memory": {
                "usage swap %": row[4],
                "used swap GB": row[5],
                "free swap GB": row[6]
            },
            "timestamp": row[7]
        } for i, row in enumerate(rows)]

    def _get_disk_info(self, cursor, machine_id, limit):
        cursor.execute('''
            SELECT * FROM MachineDisk 
            WHERE id = ? ORDER BY Timestamp DESC LIMIT ?
        ''', (machine_id, limit))
        rows = cursor.fetchall()
        return [{
            "row": i + 1,
            "disk usage": {
                "usage %": row[1],
                "used GB": row[2],
                "free GB": row[3]
            },
            "disk i/o": {
                "read_count": row[4],
                "write_count": row[5],
                "read_bytes GB": row[6],
                "write_bytes GB": row[7]
            },
            "timestamp": row[8]
        } for i, row in enumerate(rows)]

    def _get_network_info(self, cursor, machine_id, limit):
        cursor.execute('''
            SELECT * FROM MachineNetwork 
            WHERE id = ? ORDER BY Timestamp DESC LIMIT ?
        ''', (machine_id, limit))
        rows = cursor.fetchall()
        return [{
            "row": i + 1,
            "i/o": {
                "packets_sent": row[1],
                "packets_recv": row[2],
                "bytes_sent": row[3],
                "bytes_recv": row[4],
                "errors in": row[5],
                "errors out": row[6],
                "pack drop in": row[7],
                "pack drop out": row[8]
            },
            "connections": row[9],
            "timestamp": row[10]
        } for i, row in enumerate(rows)]

    def _get_table_data(self, cursor, table, machine_id, limit, ip):
        get_methods = {
            'basic_info': self._get_basic_info,
            'cpu': self._get_cpu_info,
            'memory': self._get_memory_info,
            'disks': self._get_disk_info,
            'network': self._get_network_info
        }
        
        if table not in get_methods:
            raise ValueError(f"Unknown table type: {table}")
            
        rows = get_methods[table](cursor, machine_id, limit)
        return {
            "machineIp": ip,
            "rows": rows
        }, True

    def close(self):
        if hasattr(self._local, 'conn'):
            self._local.conn.close()
            del self._local.conn
