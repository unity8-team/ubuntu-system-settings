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

import dbus
import dbusmock

BUS_NAME = 'com.canonical.indicator.network'
MAIN_IFACE = 'org.gtk.Actions'
MAIN_OBJ = '/com/canonical/indicator/network'
SYSTEM_BUS = False

NOT_IMPLEMENTED = '''raise dbus.exceptions.DBusException(
    "org.ofono.Error.NotImplemented")'''

_parameters = {}


def activate(self, action_name, parameters, platform_data):

    if action_name == 'wifi.enable':
        enabled, sig, states = self.actions[action_name]
        state = dbus.Boolean(not states[0], variant_level=1)
        new_struct = (enabled, sig, [state])
        self.actions[action_name] = new_struct
        self.Changes(dbus.Array([], signature='s'),
                     dbus.Dictionary({}, signature='sb'),
                     dbus.Dictionary({action_name: state}, signature='sv'),
                     dbus.Dictionary({}, signature='s(bgav)'))


def describe(self, action_name):
    return self.actions[action_name]


def describe_all(self):
    return self.actions


def list_actions(self):
    return list(self.actions)


def set_state(self, action_name, parameters, platform_data):
    eval(NOT_IMPLEMENTED)


@dbus.service.method(dbusmock.MOCK_IFACE,
                     in_signature='asa{sb}a{sv}a{s(bgav)}', out_signature='')
def Changes(self, removals, enable_changes, state_changes, additions):
    obj = dbusmock.get_object(MAIN_OBJ)
    obj.EmitSignal(MAIN_IFACE, 'Changed', 'asa{sb}a{sv}a{s(bgav)}', [
        removals, enable_changes, state_changes, additions
    ])


def load(mock, parameters):
    global _parameters
    _parameters = parameters

    mock.activate = activate
    mock.describe = describe
    mock.describe_all = describe_all
    mock.list_actions = list_actions
    mock.set_state = set_state

    mock.actions = _parameters.get('actions', {
        'wifi.enable': (True, '', [True]),
    })

    mock.AddMethods(
        MAIN_IFACE,
        [
            (
                'Activate', 'sava{sv}', '',
                'self.activate(self, args[0], args[1], args[2])'
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
            ),
            (
                'SetState', 'sva{sv}', '',
                'self.set_state(self, args[0], args[1], args[2])'
            )
        ])
