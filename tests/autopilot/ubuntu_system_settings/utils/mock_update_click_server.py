import json
import logging
import threading

from http.server import BaseHTTPRequestHandler, HTTPServer

logger = logging.getLogger(__name__)


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
        try:
            self.wfile.write(json.dumps(response).encode('utf-8'))
        except BrokenPipeError:
            # System Settings shut down before we finished up. Log and ignore.
            self.log_message('Server was interrupted.')


class Manager(object):

    # TODO: Use server_port=0
    def __init__(self, server_address='', server_port=9009, responses={}):
        """Creates and initializes a Manager object. If there's an asterisk
        in the responses dict, it's used to handle all paths."""
        self._thread = None
        if not responses:
            responses = {
                '*': [{
                    "name": "com.ubuntu.developer.testclick",
                    "version": "2.0",
                    "icon_url": (
                        "https://raw.githubusercontent.com/ninja-ide/"
                        "ninja-ide/master/ninja_ide/img/ninja_icon.png"
                    ),
                    "download_url": ("http://localhost:9009/download"),
                    "binary_filesize": 9000,
                    "download_sha512": "1232223sdfdsffs",
                    "changelog": "New version!"
                }]
            }
        self._httpd = HTTPServer((server_address, server_port), Handler)
        self._httpd.responses = responses
        logger.debug('Created mock update click server.')

    def is_running(self):
        return self._thread.is_alive()

    def start(self):
        self._thread = threading.Thread(target=self._httpd.serve_forever)
        self._thread.start()
        logger.debug(
            'Started mock update click server on %s:%d.' % (
                self._httpd.server_address
            )
        )

    def stop(self):
        self._httpd.shutdown()
        self._httpd.server_close()
        self._thread.join(timeout=10.0)
        if self.is_running():
            raise 'Failed to stop server'
        logger.debug('Stopped mock update click server.')
