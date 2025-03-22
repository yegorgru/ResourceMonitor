from flask import Flask, request, jsonify, make_response
from flask_cors import CORS
import logging
from DatabaseManager import DatabaseManager
from werkzeug.serving import WSGIRequestHandler

class FlaskServer:
    def __init__(self):
        self.logger = logging.getLogger(__name__)
        self.app = Flask(__name__)
        CORS(self.app)  # Enable CORS
        self.db_manager = DatabaseManager()
        
        # Force HTTP/1.1
        WSGIRequestHandler.protocol_version = "HTTP/1.1"
        
        # Register routes
        self.app.add_url_rule('/<path:path>', 
                            view_func=self._handle_request,
                            methods=['GET', 'PUT'])
        
        # Set debug level for more detailed logging
        self.logger.setLevel(logging.DEBUG)

    def _make_response(self, data, status_code=200):
        response = make_response(jsonify(data), status_code)
        response.headers['Server'] = 'Flask/HTTP/1.1'
        return response

    def _handle_request(self, path):
        # Debug logging for request details
        self.logger.debug(f"Received request: {request.method} {path}")
        self.logger.debug(f"Headers: {dict(request.headers)}")
        self.logger.debug(f"Content-Type: {request.content_type}")
        self.logger.debug(f"Data: {request.get_data()}")
        
        if request.method == 'GET':
            result, success = self.db_manager.get_machine_state(path)
            if success:
                return self._make_response(result)
            return self._make_response({"error": "not found"}, 404)
        
        elif request.method == 'PUT':
            try:
                # More lenient content type check
                content_type = request.content_type or ''
                if 'application/json' not in content_type.lower():
                    error_msg = f"Invalid content type. Expected application/json but got {content_type}"
                    self.logger.error(error_msg)
                    return self._make_response({"error": error_msg}, 415)

                try:
                    machine_data = request.get_json(force=True)  # Try to parse JSON even if content-type is wrong
                except Exception as e:
                    self.logger.error(f"Failed to parse JSON: {e}")
                    return self._make_response({"error": "Invalid JSON data"}, 400)

                if not machine_data:
                    self.logger.error("Empty JSON data received")
                    return self._make_response({"error": "Empty JSON data"}, 400)

                self.logger.debug(f"Parsed JSON data: {machine_data}")
                
                if self.db_manager.save_machine_state(machine_data, path):
                    return self._make_response({"status": "success"})
                return self._make_response({"error": "database error"}, 500)
            except Exception as e:
                error_msg = f"Error processing request: {str(e)}"
                self.logger.error(error_msg)
                return self._make_response({"error": error_msg}, 500)

    def close(self):
        self.db_manager.close()
