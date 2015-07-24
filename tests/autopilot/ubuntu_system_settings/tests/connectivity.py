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
MAIN_IFACE = 'com.ubuntu.connectivity1'
MAIN_OBJ = '/'
SYSTEM_BUS = False

PRIV_IFACE = 'com.ubuntu.connectivity1.Private'
PRIV_OBJ = '/com/ubuntu/connectivity1/Private'

NETS_IFACE = 'com.ubuntu.connectivity1.NetworkingStatus'
NETS_OBJ = '/com/ubuntu/connectivity1/NetworkingStatus'

NOT_IMPLEMENTED = '''raise dbus.exceptions.DBusException(
    "org.ofono.Error.NotImplemented")'''

_parameters = {}


def set_hotspot_enabled(self, value):
    self.SetProperty(NETS_OBJ, NETS_IFACE, 'HotspotEnabled', value)

    # Set HotspotStored = True if not stored and we're enabling it.
    stored = dbusmock.get_object(NETS_OBJ).Get(NETS_IFACE, 'HotspotStored')
    if value and not bool(stored):
        self.SetProperty(NETS_OBJ, NETS_IFACE, 'HotspotStored', True)


def set_hotspot_ssid(self, value):
    self.SetProperty(NETS_OBJ, NETS_IFACE, 'HotspotSsid', value)


def set_hotspot_password(self, value):
    self.SetProperty(PRIV_OBJ, PRIV_IFACE, 'HotspotPassword', value)


def load(mock, parameters):
    global _parameters
    _parameters = parameters

    mock.set_hotspot_enabled = set_hotspot_enabled
    mock.set_hotspot_ssid = set_hotspot_ssid
    mock.set_hotspot_password = set_hotspot_password

    mock.modems = []  # path to boolean e.g. ril_0: False
    mock.flight_mode = False
    mock.wifi_enabled = False

    mock.AddObject(
        NETS_OBJ,
        NETS_IFACE,
        {
            'HotspotSsid': _parameters.get(
                'HotspotSsid', dbus.ByteArray('Ubuntu'.encode('UTF-8'))),
            'HotspotEnabled': _parameters.get(
                'HotspotEnabled', dbus.Boolean(False)),
            'HotspotMode': _parameters.get('HotspotMode', dbus.String('ap')),
            'HotspotStored': _parameters.get(
                'HotspotStored', dbus.Boolean(False)
            ),
            'UnstoppableOperationHappening': dbus.Boolean(False),
        },
        []
    )

    mock.AddObject(
        PRIV_OBJ,
        PRIV_IFACE,
        {
            'HotspotPassword': _parameters.get(
                'HotspotPassword', dbus.String('abcdefgh')
            )
        },
        [
            (
                'UnlockAllModems', '', '',
                ''
            ),
            (
                'UnlockModem', 's', '',
                ''
            ),
            (
                'SetFlightMode', 'b', '',
                ''
            ),
            (
                'SetWifiEnabled', 'b', '',
                ''
            ),
            (
                'SetHotspotSsid', 'ay', '',
                'objects["/"].set_hotspot_ssid(self, args[0])'
            ),
            (
                'SetHotspotPassword', 's', '',
                'objects["/"].set_hotspot_password(self, args[0])'
            ),
            (
                'SetHotspotEnabled', 'b', '',
                'objects["/"].set_hotspot_enabled(self, args[0])'
            ),
            (
                'SetHotspotMode', 's', '',
                ''
            )
        ]
    )


@dbus.service.method(dbusmock.MOCK_IFACE,
                     in_signature='sssv', out_signature='')
def SetProperty(self, path, iface, name, value):
    obj = dbusmock.get_object(path)
    obj.Set(iface, name, value)
    obj.EmitSignal(iface, 'PropertiesChanged', 'a{sv}', [{name: value}])
