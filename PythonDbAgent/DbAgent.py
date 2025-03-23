import threading
import logging
import signal
import sys
from config import HOST, PORT, LOG_LEVEL, LOG_FORMAT
from FlaskServer import FlaskServer
from werkzeug.serving import make_server, WSGIRequestHandler

class DbAgent:
    def __init__(self):
        logging.basicConfig(level=LOG_LEVEL, format=LOG_FORMAT)
        self.logger = logging.getLogger(__name__)
        self.running = False
        self.server_active = False
        self.server_thread = None
        self.stop_event = threading.Event()
        self.flask_server = FlaskServer()
        self.server = None
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
            if self.server_active and self.server:
                self.logger.info("Shutting down server...")
                self.server.shutdown()
                self.server_active = False
            self.stop_event.set()
            
            if self.server_thread:
                self.server_thread.join(timeout=2)
                self.server_thread = None
                self.server = None
        
    def start_server(self):
        if self.server_active:
            self.logger.warning("Server is already running")
            return False
            
        self.server = make_server(HOST, PORT, self.flask_server.app, request_handler=WSGIRequestHandler)
        self.server_thread = threading.Thread(
            target=self.server.serve_forever
        )
        self.server_thread.start()
        self.server_active = True
        self.logger.info(f"Server started on {HOST}:{PORT}")
        return True

    def handle_user_input(self):
        while self.running:
            command = input("Enter command (start/stop/exit): ").strip().lower()
            if command == "start":
                if self.start_server():
                    self.logger.info("Server is now accepting requests")
            elif command == "stop":
                if self.server_active:
                    self.logger.info("Stopping server...")
                    self.server.shutdown()
                    self.server_active = False
                    if self.server_thread:
                        self.server_thread.join(timeout=2)
                        self.server_thread = None
                        self.server = None
                    self.logger.info("Server stopped")
                else:
                    self.logger.warning("Server is not running")
            elif command == "exit":
                self.stop()
                break
            else:
                self.logger.warning("Unknown command")
        
    def run(self):
        self.running = True
        try:
            self.handle_user_input()
        except KeyboardInterrupt:
            self.stop()
        finally:
            if self.server_thread and self.server_thread.is_alive():
                self.stop()
            self.flask_server.close()
            self.logger.info("Application terminated")
            sys.exit(0)