import threading
import requests
import logging
import signal
from ResourceMonitor import ResourceMonitor
from config import ENDPOINTS, UPDATE_INTERVAL

class ResourceAgent:
    def __init__(self):
        logging.basicConfig(level=logging.INFO)
        self.logger = logging.getLogger(__name__)
        self.monitor = ResourceMonitor()
        self.running = False
        self.send_requests = False
        self.first_req = True
        self.stop_event = threading.Event()
        self._setup_signal_handlers()

    def _setup_signal_handlers(self):
        signal.signal(signal.SIGINT, self._signal_handler)
        signal.signal(signal.SIGTERM, self._signal_handler)
        
    def _signal_handler(self, signum, frame):
        self.logger.info(f"Received signal {signum}")
        self.stop()

    def stop(self):
        if self.running:
            self.running = False
            if self.send_requests:
                self.send_requests = False
                self.logger.info("Stopping data collection...")
            self.stop_event.set()

    def send_stats(self):
        while self.running:
            if self.send_requests:
                try:
                    if self.first_req:
                        self.first_req = not self._send_request("basic_info", self.monitor.get_basic_stats())
                    if not self.first_req:
                        # Collect and send stats
                        self._send_request("cpu", self.monitor.get_cpu_stats())
                        self._send_request("memory", self.monitor.get_memory_stats())
                        self._send_request("disks", self.monitor.get_disk_stats())
                        self._send_request("network", self.monitor.get_network_stats())
                except Exception as e:
                    self.logger.error(f"Error collecting/sending stats: {e}")
                
                # Wait after sending data
                for _ in range(UPDATE_INTERVAL):
                    if self.stop_event.wait(timeout=1):
                        return
                    if not self.running or not self.send_requests:
                        break
            else:
                # When not sending, just check every second
                if self.stop_event.wait(timeout=1):
                    return

    def _send_request(self, endpoint_name, data):
        try:
            headers = {
                'Content-Type': 'application/json',
                'Accept': 'application/json'
            }
            self.logger.debug(f"Sending request to {endpoint_name} with headers: {headers}")
            self.logger.debug(f"Data: {data}")
            
            response = requests.put(
                ENDPOINTS[endpoint_name],
                json=data,
                headers=headers
            )
            
            self.logger.debug(f"Response status: {response.status_code}")
            self.logger.debug(f"Response headers: {dict(response.headers)}")
            self.logger.debug(f"Response body: {response.text}")
            
            if response.status_code == 200:
                self.logger.info(f"{endpoint_name} info sent successfully")
                return True
            else:
                self.logger.error(f"Failed to send {endpoint_name} info. Status: {response.status_code}, Response: {response.text}")
                return False
        except Exception as e:
            self.logger.error(f"Error sending {endpoint_name} info: {str(e)}")

    def handle_user_input(self):
        while self.running:
            command = input("Enter command (start/stop/exit): ").strip().lower()
            if command == "start":
                if not self.send_requests:
                    self.send_requests = True
                    self.first_req = True  # Reset first_req when starting
                    self.logger.info("Started collecting and sending data")
                else:
                    self.logger.warning("Data collection is already running")
            elif command == "stop":
                if self.send_requests:
                    self.send_requests = False
                    self.logger.info("Stopped data collection")
                else:
                    self.logger.warning("Data collection is not running")
            elif command == "exit":
                self.stop()
                break
            else:
                self.logger.warning("Unknown command")

    def run(self):
        self.running = True
        data_thread = threading.Thread(target=self.send_stats)
        data_thread.start()
        
        try:
            self.handle_user_input()
        except KeyboardInterrupt:
            self.stop()
        finally:
            data_thread.join()
            self.logger.info("Application terminated")
