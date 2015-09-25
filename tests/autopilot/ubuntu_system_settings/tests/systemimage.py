'''system image D-BUS mock template'''

# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU Lesser General Public License as published by the Free
# Software Foundation; either version 3 of the License, or (at your option) any
# later version.  See http://www.gnu.org/copyleft/lgpl.html for the full text
# of the license.
import dbus

__author__ = 'Jonas G. Drange'
__email__ = 'jonas.drange@canonical.com'
__copyright__ = '(c) 2015 Canonical Ltd.'
__license__ = 'LGPL 3+'

BUS_NAME = 'com.canonical.SystemImage'
MAIN_IFACE = 'com.canonical.SystemImage'
MAIN_OBJ = '/Service'
SYSTEM_BUS = True


def load(mock, parameters):
    global _parameters
    _parameters = parameters

    mock.props = {
        'build_number': _parameters.get('build_number', 0),
        'device': _parameters.get('device', ''),
        'channel': _parameters.get('channel', ''),
        'last_update_date': _parameters.get('last_update_date', ''),
        'last_check_date': _parameters.get('last_check_date', ''),
        'target_build_number': _parameters.get('target_build_number', -1),
        'target_version_detail': _parameters.get('target_version_detail', ''),
        'version_detail': _parameters.get(
            'version_detail', dbus.Dictionary({}, signature='ss')
        )
    }


@dbus.service.method(MAIN_IFACE,
                     in_signature='', out_signature='isssa{ss}')
def Info(self):
    return (
        self.props['build_number'],
        self.props['device'],
        self.props['channel'],
        self.props['last_update_date'],
        self.props['version_detail']
    )


@dbus.service.method(MAIN_IFACE,
                     in_signature='', out_signature='a{ss}')
def Information(self):
    return dbus.Dictionary({}, signature='ss')
