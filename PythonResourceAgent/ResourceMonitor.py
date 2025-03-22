import psutil
from config import MACHINE_IP

class ResourceMonitor:
    def get_basic_stats(self):
        virtual_memory = psutil.virtual_memory()
        swap_memory = psutil.swap_memory()
        return {
            "ip": MACHINE_IP,
            "numcpus": psutil.cpu_count(),
            "total virt mem GB": virtual_memory.total / (1024 ** 3),
            "total swap mem GB": swap_memory.total / (1024 ** 3),
            "numdisks": len(psutil.disk_partitions()),
            "total_C_disk GB": psutil.disk_usage('/').total / (1024 ** 3)
        }

    def get_cpu_stats(self):
        cpu_times = psutil.cpu_times()
        cpu_freq = psutil.cpu_freq()
        return {
            "ip": MACHINE_IP,
            "cpu_times": {
                "cpu_user": cpu_times.user,
                "cpu_system": cpu_times.system,
                "cpu_idle": cpu_times.idle
            },
            "cpu_usage %": psutil.cpu_percent(interval=1),
            "cpu_freq": {
                "freq_curr Mhz": cpu_freq.current,
                "freq_min Mhz": cpu_freq.min,
                "freq_max Mhz": cpu_freq.max
            }
        }

    def get_memory_stats(self):
        virtual_memory = psutil.virtual_memory()
        swap_memory = psutil.swap_memory()
        return {
            "ip": MACHINE_IP,
            "virt_memory": {
                "usage virt %": virtual_memory.percent,
                "used virt GB": virtual_memory.used / (1024 ** 3),
                "available virt GB": virtual_memory.available / (1024 ** 3)
            },
            "swap_memory": {
                "usage swap %": swap_memory.percent,
                "used swap GB": swap_memory.used / (1024 ** 3),
                "free swap GB": swap_memory.free / (1024 ** 3)
            }
        }

    def get_disk_stats(self):
        disk_usage = psutil.disk_usage('/')
        disk_io = psutil.disk_io_counters()
        return {
            "ip": MACHINE_IP,
            "disk usage": {
                "usage %": disk_usage.percent,
                "used GB": disk_usage.used / (1024 ** 3),
                "free GB": disk_usage.free / (1024 ** 3)
            },
            "disk i/o": {
                "read_count": disk_io.read_count,
                "write_count": disk_io.write_count,
                "read_bytes GB": disk_io.read_bytes / (1024 ** 3),
                "write_bytes GB": disk_io.write_bytes / (1024 ** 3)
            }
        }

    def get_network_stats(self):
        network_io = psutil.net_io_counters()
        return {
            "ip": MACHINE_IP,
            "i/o": {
                "packets_sent": network_io.packets_sent,
                "packets_recv": network_io.packets_recv,
                "bytes_sent": network_io.bytes_sent,
                "bytes_recv": network_io.packets_recv,
                "errors in": network_io.errin,
                "errors out": network_io.errout,
                "pack drop in": network_io.dropin,
                "pack drop out": network_io.dropout
            },
            "connections": len(psutil.net_connections())
        }
