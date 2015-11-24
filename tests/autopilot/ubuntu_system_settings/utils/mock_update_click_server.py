import json
import threading
from http.server import BaseHTTPRequestHandler, HTTPServer


class ServerThread(threading.Thread):

    def __init__(self, **kwargs):
        super(ServerThread, self).__init__()
        handler = Handler

        if not kwargs['responses']:
            responses = {
                '*': [{
                    "name": "com.ubuntu.developer.testclick",
                    "version": "2.0",
                    "icon_url": (
                        "https://raw.githubusercontent.com/ninja-ide/"
                        "ninja-ide/master/ninja_ide/img/ninja_icon.png"
                    ),
                    "download_url": ("http://localhost:8000/download"),
                    "binary_filesize": 9000,
                    "download_sha512": "1232223sdfdsffs",
                    "changelog": "New version!"
                }]
            }
        self._httpd = HTTPServer(
             (kwargs['server_address'], kwargs['server_port']), handler
        )
        self._httpd.responses = responses

    def run(self):
        self._httpd.serve_forever()

    def stop(self):
        # Stop the run loop in server_forever
        self._httpd.shutdown()
        # Clean up.
        self._httpd.server_close()


class Handler(BaseHTTPRequestHandler):

    responses = {}

    def do_HEAD(self):
        """Sends headers.."""
        self.send_response(200)
        self.send_header("X-Click-Token", "X-Click-Token")
        self.end_headers()

    def do_POST(self):
        """Respond to a POST request."""
        self.do_GET()

    def do_GET(self):
        """Respond to a GET request."""
        if '*' in self.server.responses:
            response = self.server.responses['*']
        else:
            response = self.server.responses[self.path]
        self.send_response(200)
        self.send_header("Content-type", "application/json")
        self.end_headers()
        self.wfile.write(json.dumps(response).encode('utf-8'))


class Manager(object):

    def __init__(self, server_address='', server_port=8000, responses={}):
        """Creates and initializes a Manager object. If there's an asterisk
        in the responses dict, it's used to handle all paths."""
        self.thread = None
        self.server_address = server_address
        self.server_port = server_port
        self.responses = responses

    def is_running(self):
        return self.thread.is_alive()

    def start(self):
        self.thread = ServerThread(
            server_address=self.server_address,
            server_port=self.server_port,
            responses=self.responses
        )
        self.thread.start()

    def stop(self):
        self.thread.stop()
        self.thread.join(timeout=10)
        if self.is_running():
            raise "The server could not be stopped in time."
