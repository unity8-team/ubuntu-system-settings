'''indicator-network D-BUS mock template'''

# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU Lesser General Public License as published by the Free
# Software Foundation; either version 3 of the License, or (at your option) any
# later version.  See http://www.gnu.org/copyleft/lgpl.html for the full text
# of the license.

__author__ = 'Jonas G. Drange'
__email__ = 'jonas.drange@canonical.com'
__copyright__ = '(c) 2015 Canonical Ltd.'
__license__ = 'LGPL 3+'


BUS_NAME = 'com.canonical.indicator.network'
MAIN_IFACE = 'org.gtk.Actions'
MAIN_OBJ = '/com/canonical/indicator/network'
SYSTEM_BUS = False

NOT_IMPLEMENTED = '''raise dbus.exceptions.DBusException(
    "org.ofono.Error.NotImplemented")'''

_parameters = {}


def load(mock, parameters):
    import syslog
    syslog.syslog("inetwork: was called")
    global _parameters
    _parameters = parameters

    mock.AddMethods(
        MAIN_IFACE,
        [
            (
                'Activate', 'sava{sv}', '',
                ''
            ),
            (
                'Describe', 's', '(bgav)',
                'if args[0] == "wifi.enable":'
                '   ret = (True, "", [True])'
            ),
            (
                'DescribeAll', '', 'a{s(bgav)}',
                'ret = {"wifi.enable": (True, "", [True])}'
            ),
            (
                'List', '', 'as',
                'ret = ["wifi.enable"]'
            ),
            (
                'SetState', 'sva{sv}', '',
                ''
            )
        ])
