'''indicator-network D-BUS mock template'''

# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU Lesser General Public License as published by the Free
# Software Foundation; either version 3 of the License, or (at your option) any
# later version.  See http://www.gnu.org/copyleft/lgpl.html for the full text
# of the license.

import dbus
import dbusmock

__author__ = 'Jonas G. Drange'
__email__ = 'jonas.drange@canonical.com'
__copyright__ = '(c) 2015 Canonical Ltd.'
__license__ = 'LGPL 3+'

BUS_NAME = 'com.canonical.indicator.network'
MAIN_IFACE = 'org.gtk.Actions'
MAIN_OBJ = '/com/canonical/indicator/network'
MENU_IFACE = 'org.gtk.Menus'
PHONE_WIFI_OBJ = '/com/canonical/indicator/network/phone_wifi_settings'
SYSTEM_BUS = False

NOT_IMPLEMENTED = '''raise dbus.exceptions.DBusException(
    "org.ofono.Error.NotImplemented")'''

_parameters = {}


def activate(self, action_name, parameters, platform_data):
    pass


def describe(self, action_name):
    return self.actions[action_name]


def describe_all(self):
    return self.actions


def list_actions(self):
    return list(self.actions)


def start(self, groups):
    return dbusmock.get_object(MAIN_OBJ).menus


def end(self, groups):
    pass


@dbus.service.method(dbusmock.MOCK_IFACE,
                     in_signature='asa{sb}a{sv}a{s(bgav)}', out_signature='')
def Changes(self, removals, enable_changes, state_changes, additions):
    obj = dbusmock.get_object(MAIN_OBJ)
    obj.EmitSignal(MAIN_IFACE, 'Changed', 'asa{sb}a{sv}a{s(bgav)}', [
        removals, enable_changes, state_changes, additions
    ])
    pass


def load(mock, parameters):
    global _parameters
    _parameters = parameters

    mock.describe = describe
    mock.describe_all = describe_all
    mock.list_actions = list_actions

    mock.actions = parameters.get('actions', {
        'wifi.enable': (True, '', [True]),
        'accesspoint.0': (True, '', [True]),
        'accesspoint.0::strength': (True, '', [44]),
        'accesspoint.1': (True, '', [False]),
        'accesspoint.1::strength': (True, '', [100]),
        'accesspoint.2': (True, '', [False]),
        'accesspoint.2::strength': (True, '', [74]),
        'accesspoint.3': (False, '', [False]),
    })

    mock.menus = parameters.get('menus', dbus.Array([
        (
            dbus.UInt32(0), dbus.UInt32(0),
            [
                {
                    'action': 'indicator.wifi.enable',
                    'x-canonical-type': 'com.canonical.indicator.switch',
                    'label': 'Wi-Fi'
                },
                {
                    'x-canonical-type': 'com.canonical.indicator.section',
                    'label': 'Available Wi-Fi networks'
                },
                {
                    ':section': dbus.Struct(
                        (dbus.UInt32(0), dbus.UInt32(1)), signature='(uu)'
                    )
                }
            ]
        ),
        (
            dbus.UInt32(0), dbus.UInt32(1),
            [
                {
                    'x-canonical-wifi-ap-is-secure': True,
                    'x-canonical-wifi-ap-is-enterprise': False,
                    'label': 'Secure',
                    'x-canonical-type':
                        'unity.widgets.systemsettings.tablet.accesspoint',
                    'x-canonical-wifi-ap-strength-action':
                        'indicator.accesspoint.0::strength',
                    'action': 'indicator.accesspoint.0',
                    'x-canonical-wifi-ap-is-adhoc': False
                },
                {
                    'x-canonical-wifi-ap-is-secure': False,
                    'x-canonical-wifi-ap-is-enterprise': False,
                    'label': 'Insecure',
                    'x-canonical-type':
                        'unity.widgets.systemsettings.tablet.accesspoint',
                    'x-canonical-wifi-ap-strength-action':
                        'indicator.accesspoint.1::strength',
                    'action': 'indicator.accesspoint.1',
                    'x-canonical-wifi-ap-is-adhoc': False
                },
                {
                    'x-canonical-wifi-ap-is-secure': True,
                    'x-canonical-wifi-ap-is-enterprise': True,
                    'label': 'Enterprise',
                    'x-canonical-type':
                        'unity.widgets.systemsettings.tablet.accesspoint',
                    'x-canonical-wifi-ap-strength-action':
                        'indicator.accesspoint.2::strength',
                    'action': 'indicator.accesspoint.2',
                    'x-canonical-wifi-ap-is-adhoc': False
                },
                {
                    'x-canonical-wifi-ap-is-secure': False,
                    'x-canonical-wifi-ap-is-enterprise': False,
                    'label': 'Unknown',
                    'x-canonical-type':
                        'unknown-type',
                    'action': 'indicator.accesspoint.3',
                    'x-canonical-wifi-ap-is-adhoc': False
                }
            ]
        )
    ], signature='a(uuaa{sv})'))

    mock.AddMethods(
        MAIN_IFACE,
        [
            (
                'Activate', 'sava{sv}', '', ''
            ),
            (
                'Describe', 's', '(bgav)',
                'ret = self.describe(self, args[0])'
            ),
            (
                'DescribeAll', '', 'a{s(bgav)}',
                'ret = self.describe_all(self)'
            ),
            (
                'List', '', 'as',
                'ret = self.list_actions(self)'
            )
        ])

    mock.AddObject(
        PHONE_WIFI_OBJ,
        MENU_IFACE, {}, [
            (
                'Start', 'au', 'a(uuaa{sv})',
                'ret = self.start(self, args[0])'
            ),
            (
                'End', 'au', '',
                'ret = self.end(self, args[0])'
            )
        ]
    )

    phone_wifi_obj = dbusmock.get_object(PHONE_WIFI_OBJ)
    phone_wifi_obj.start = start
    phone_wifi_obj.end = end
