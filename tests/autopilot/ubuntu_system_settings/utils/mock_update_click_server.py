import json
import threading
from http.server import BaseHTTPRequestHandler, HTTPServer


KEEP_ALIVE = True


class MyHandler(BaseHTTPRequestHandler):

    def do_HEAD(self):
        self.send_response(200)
        self.send_header("X-Click-Token", "X-Click-Token")
        self.end_headers()

    def response_item_info(self):
        response = [{
            "name": "com.ubuntu.calculator",
            "version": "9.0",
            "icon_url": "https://raw.githubusercontent.com/ninja-ide/ninja-ide/master/ninja_ide/img/ninja_icon.png",
            "download_url": "https://public.apps.ubuntu.com/download/com.ubuntu/calculator/com.ubuntu.calculator_1.3.329_all.click",
            "binary_filesize": 3423,
            "download_sha512": "343244fsdfdsffs"
        }]
        self.send_response(200)
        self.send_header("Content-type", "application/json")
        self.end_headers()
        self.wfile.write(bytes(json.dumps(response), 'UTF-8'))

    def do_POST(self):
        """Respond to a POST request."""
        self.do_GET()

    def do_GET(self):
        """Respond to a GET request."""
        if self.path.find("iteminfo/") != -1:
            self.response_item_info()
        elif self.path.find("shutdown") != -1:
            global KEEP_ALIVE
            KEEP_ALIVE = False


def run_click_server():
    server_address = ('', 8000)
    httpd = HTTPServer(server_address, MyHandler)
    global KEEP_ALIVE
    print('start')
    while KEEP_ALIVE:
        httpd.handle_request()


def run_mocked_settings():
    t = threading.Thread(target=run_click_server)
    t.start()


run_mocked_settings()
