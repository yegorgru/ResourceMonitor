from flask import Flask, request, jsonify, make_response
from flask_cors import CORS
import logging
from DatabaseManager import DatabaseManager
from werkzeug.serving import WSGIRequestHandler

class FlaskServer:
    def __init__(self):
        self.logger = logging.getLogger(__name__)
        self.app = Flask(__name__)
        CORS(self.app)
        self.db_manager = DatabaseManager()
        
        WSGIRequestHandler.protocol_version = "HTTP/1.1"
        
        self.app.add_url_rule('/<path:path>', 
                            view_func=self._handle_request,
                            methods=['GET', 'PUT'])

    def _make_response(self, data, status_code=200):
        response = make_response(jsonify(data), status_code)
        return response

    def _handle_request(self, path):
        self.logger.debug(f"Received request: {request.method} {path}")
        self.logger.debug(f"Headers: {dict(request.headers)}")
        self.logger.debug(f"Content-Type: {request.content_type}")
        self.logger.debug(f"Data: {request.get_data()}")
        
        if request.method == 'GET':
            result, success = self.db_manager.get_machine_state(path)
            if success:
                self.logger.info(f"Successfully retrieved data for path: {path}")
                return self._make_response(result)
            self.logger.warning(f"No data found for path: {path}")
            return self._make_response({"error": "not found"}, 404)
        
        elif request.method == 'PUT':
            try:
                content_type = request.content_type or ''
                if 'application/json' not in content_type.lower():
                    error_msg = f"Invalid content type. Expected application/json but got {content_type}"
                    self.logger.error(error_msg)
                    return self._make_response({"error": error_msg}, 415)

                try:
                    machine_data = request.get_json(force=True)
                except Exception as e:
                    self.logger.error(f"Failed to parse JSON: {e}")
                    return self._make_response({"error": "Invalid JSON data"}, 400)

                if not machine_data:
                    self.logger.error("Empty JSON data received")
                    return self._make_response({"error": "Empty JSON data"}, 400)

                self.logger.debug(f"Parsed JSON data: {machine_data}")
                
                if self.db_manager.save_machine_state(machine_data, path):
                    self.logger.info(f"Successfully saved data for path: {path}")
                    return self._make_response({"status": "success"})
                self.logger.error(f"Database error while saving data for path: {path}")
                return self._make_response({"error": "database error"}, 500)
            except Exception as e:
                error_msg = f"Error processing request: {str(e)}"
                self.logger.error(error_msg)
                return self._make_response({"error": error_msg}, 500)

    def close(self):
        self.db_manager.close()
