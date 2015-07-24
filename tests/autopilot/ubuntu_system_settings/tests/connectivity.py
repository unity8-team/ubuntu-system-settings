'''connectivity D-BUS mock template'''

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

BUS_NAME = 'com.ubuntu.connectivity1'
MAIN_IFACE = 'com.ubuntu.connectivity1.NetworkingStatus'
MAIN_OBJ = '/com/ubuntu/connectivity1/NetworkingStatus'
SYSTEM_BUS = False

PRIV_IFACE = 'com.ubuntu.connectivity1.Private'
PRIV_OBJ = '/com/ubuntu/connectivity1/Private'

NETS_IFACE = MAIN_IFACE
NETS_OBJ = MAIN_OBJ

NOT_IMPLEMENTED = '''raise dbus.exceptions.DBusException(
    "org.ofono.Error.NotImplemented")'''

_parameters = {}


def load(mock, parameters):
    global _parameters
    mock.modems = []  # path to boolean e.g. ril_0: False
    mock.flight_mode = False
    mock.wifi_enabled = False
    _parameters = parameters

    nets = dbusmock.get_object(NETS_OBJ)
    nets.AddProperties(NETS_IFACE, {
        'HotspotSsid': _parameters.get(
            'HotspotSsid', dbus.ByteArray('Ubuntu'.encode('UTF-8'))),
        'HotspotEnabled': _parameters.get(
            'HotspotEnabled', dbus.Boolean(False)),
        'HotspotMode': _parameters.get('HotspotMode', dbus.String('ap')),
        'HotspotStored': _parameters.get('HotspotStored', dbus.Boolean(False)),
        'UnstoppableOperationHappening': dbus.Boolean(False),
    })

    mock.AddObject(PRIV_OBJ, PRIV_IFACE, {
        'HotspotPassword': _parameters.get(
            'HotspotPassword', dbus.String('abcdefgh')
        )
    }, [])


@dbus.service.method(NETS_IFACE,
                     in_signature='sssv', out_signature='')
def SetProperty(self, path, iface, name, value):
    import syslog
    syslog.syslog("Set prop %s %s %s %s" % (path, iface, name, value))
    obj = dbusmock.get_object(path)
    obj.Set(iface, name, value)
    obj.EmitSignal(iface, 'PropertiesChanged', 'a{sv}', [{name: value}])


@dbus.service.method(PRIV_IFACE, in_signature='', out_signature='')
def UnlockAllModems(self):
    '''Unlocks all modems.'''
    pass


@dbus.service.method(PRIV_IFACE, in_signature='s', out_signature='')
def UnlockModem(self, modem):
    '''Unlocks a modem.'''
    pass


@dbus.service.method(PRIV_IFACE, in_signature='b', out_signature='')
def SetFlightMode(self, enabled):
    '''Sets flight mode.'''
    pass


@dbus.service.method(PRIV_IFACE, in_signature='b', out_signature='')
def SetWifiEnabled(self, enabled):
    '''Sets wifi enabled.'''
    pass


@dbus.service.method(PRIV_IFACE, in_signature='ay', out_signature='')
def SetHotspotSsid(self, ssid):
    '''Sets the hotspot ssid.'''
    pass


@dbus.service.method(PRIV_IFACE, in_signature='s', out_signature='')
def SetHotspotPassword(self, password):
    '''Sets hotspot password.'''
    pass


@dbus.service.method(PRIV_IFACE, in_signature='b', out_signature='')
def SetHotspotEnabled(self, enabled):
    '''Sets hotspot enabled.'''
    import syslog
    syslog.syslog("foo: sat enabled" + str(enabled) + " - " + enabled)
    self.SetProperty(NETS_OBJ, NETS_IFACE, 'HotspotEnabled', True)


@dbus.service.method(PRIV_IFACE, in_signature='s', out_signature='')
def SetHotspotMode(self, mode):
    '''Sets the hotspot mode.'''
    pass
