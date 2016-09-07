import argparse
import json
import sys
import threading

from datetime import datetime
from http.server import BaseHTTPRequestHandler, HTTPServer
from urllib.parse import urlparse


def log(msg):
    fd = sys.stdout
    fd.write('%s %s\n' % (datetime.now().strftime('%H:%M:%S'), msg))
    fd.flush()


class Handler(BaseHTTPRequestHandler):

    responses = {}

    def do_HEAD(self):
        """Sends headers.."""
        self.send_response(200)
        self.send_header("X-Click-Token", "Mock-X-Click-Token")
        self.end_headers()

    def do_POST(self):
        """Respond to a POST request."""
        self.do_GET()

    def do_GET(self):
        """Respond to a GET request."""
        r = None
        o = urlparse(self.path)
        for response in self.server.responses:
            if '*' in response['path']:
                r = response
            elif o.path in response['path']:
                r = response

        if not r:
            self.log_message('Could not handle request')
            print(o.path)
            return

        self.send_response(r['status_code'])
        self.send_header("Content-type", 'application/json')
        self.end_headers()

        if 'content' in r:
            try:
                self.wfile.write(
                    json.dumps(r['content']).encode('utf-8')
                )
            except BrokenPipeError:
                # System Settings shut down before we finished
                # up. Log and ignore.
                self.log_message('Server was interrupted.')


class Manager(object):

    # TODO: Use server_port=0
    def __init__(self, server_address='', server_port=9009, responses={},
                 cmdline=False):
        """Creates and initializes a Manager object. If there's an asterisk
        in the responses dict, it's used to handle all paths."""
        self._thread = None
        self._cmdline = cmdline
        if not responses:
            responses = [
                {
                    'path': '/metadata',
                    'status_code': 200,
                    'content_type': 'application/json',
                    'content': [{
                        "name": "com.ubuntu.dropping-letters",
                        "version": "0.1.2.3",
                        "icon_url": (
                            "https://raw.githubusercontent.com/ninja-ide/"
                            "ninja-ide/master/ninja_ide/img/ninja_icon.png"
                        ),
                        "download_url": ("https://upload.wikimedia.org/wikiped"
                                         "ia/commons/3/30/04U_Dec_28_2012_0230"
                                         "Z.jpg"),
                        "binary_filesize": 23820.0 * 1000.0,
                        "download_sha512": "foo",
                        "changelog": "New Dropping Letters.",
                        "title": "Dropping Letters game"
                    }, {
                        "name": "com.ubuntu.stock-ticker-mobile",
                        "version": "0.3.7ubuntu2",
                        "icon_url": (
                            "https://upload.wikimedia.org/wikipedia/"
                            "commons/a/ab/Logo-ubuntu_cof-orange-hex.svg"
                        ),
                        "download_url": ("https://upload.wikimedia.org/wikiped"
                                         "ia/commons/d/d2/The_Harvesters%2C_p"
                                         "ainting_by_Brugel%2C_with_frame.jpg"
                                         ),
                        "binary_filesize": 5015.2 * 1000.0,
                        "download_sha512": "bar",
                        "changelog": "New ticker.",
                        "title": "A stock trading app with charts, news, and management"  # noqa
                    }]
                },
                {
                    'path': '/403',
                    'status_code': 403
                },
                {
                    'path': '/404',
                    'status_code': 404
                },
            ]
        self._httpd = HTTPServer((server_address, server_port), Handler)
        self._httpd.responses = responses
        log('Created mock update click server.')

    def is_running(self):
        return self._thread.is_alive()

    def start(self):
        self._thread = threading.Thread(target=self._httpd.serve_forever)
        self._thread.start()
        log(
            'Started mock update click server on http://%s:%d.' % (
                self._httpd.server_address
            )
        )

        # If the command line is the caller, wait for the keyboard interrupt.
        # TODO: infer this by checking sys?
        if self._cmdline:
            print('Ctrl-C stops this server.')
            try:
                while self.is_running():
                    self._thread.join(5)
            except (KeyboardInterrupt, SystemExit):
                print('')
                self.stop()

    def stop(self):
        self._httpd.shutdown()
        self._httpd.server_close()
        self._thread.join(timeout=10.0)
        if self.is_running():
            raise 'Failed to stop server'
        log('Stopped mock update click server.')


def parse_args():
    parser = argparse.ArgumentParser(description='mock click update server')

    parser.add_argument('-a', '--address', default='localhost',
                        help='address of server (default: localhost)')
    parser.add_argument('-p', '--port', type=int, default=9009,
                        help='port of server (default: 9009)')

    parser.add_argument('-r', '--responses',
                        help='JSON dictionary of path to responses. '
                             'Passing a * (asteriks) path will used '
                             'in all responses.')

    args = parser.parse_args()

    return args


if __name__ == '__main__':
    args = parse_args()

    responses = None
    if args.responses:
        try:
            responses = json.loads(args.responses)
        except ValueError as detail:
            sys.stderr.write('Malformed JSON given for '
                             'responses: %s\n' % detail)
            sys.exit(2)

        if not isinstance(responses, list):
            sys.stderr.write('JSON responses must be a list\n')
            sys.exit(2)

    man = Manager(
        server_address=args.address,
        server_port=args.port,
        responses=responses,
        cmdline=True)

    man.start()
