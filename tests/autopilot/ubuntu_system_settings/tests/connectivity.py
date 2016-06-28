'''connectivity D-BUS mock template'''

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

BUS_NAME = 'com.ubuntu.connectivity1'
MAIN_IFACE = 'com.ubuntu.connectivity1'
MAIN_OBJ = '/'
SYSTEM_BUS = False

PRIV_IFACE = 'com.ubuntu.connectivity1.Private'
PRIV_OBJ = '/com/ubuntu/connectivity1/Private'

MODEM_IFACE = 'com.ubuntu.connectivity1.Modem'
SIM_IFACE = 'com.ubuntu.connectivity1.Sim'

NETS_IFACE = 'com.ubuntu.connectivity1.NetworkingStatus'
NETS_OBJ = '/com/ubuntu/connectivity1/NetworkingStatus'

VPN_OBJ = '/com/ubuntu/connectivity1/vpn'
VPN_CONN_IFACE = 'com.ubuntu.connectivity1.vpn.VpnConnection'
VPN_CONN_OPENVPN_IFACE = 'com.ubuntu.connectivity1.vpn.VpnConnection.OpenVpn'
VPN_CONN_PPTP_IFACE = 'com.ubuntu.connectivity1.vpn.VpnConnection.Pptp'

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


def set_hotspot_auth(self, value):
    self.SetProperty(PRIV_OBJ, PRIV_IFACE, 'HotspotAuth', value)


def set_wifi_enabled(self, value):
    self.SetProperty(NETS_OBJ, NETS_IFACE, 'WifiEnabled', value)


@dbus.service.method(dbusmock.MOCK_IFACE,
                     in_signature='s', out_signature='s')
def AddSim(self, iccid):
    path = "/com/ubuntu/connectivity1/sim/{}".format(iccid)
    self.AddObject(
        path,
        SIM_IFACE,
        {
            'Iccid': dbus.String(iccid),
            'PrimaryPhoneNumber': dbus.String("358401234567"),
            'Locked': dbus.Boolean(False),
            'Present': dbus.Boolean(True),
            'Mcc': dbus.String("358"),
            'Mnc': dbus.String("42"),
            'PreferredLanguages': [dbus.String("en"), dbus.String("fi")],
            'DataRoamingEnabled': dbus.Boolean(False),
        },
        [
            ('Unlock', '', '', ''),
        ]
    )
    return path


@dbus.service.method(dbusmock.MOCK_IFACE,
                     in_signature='sis', out_signature='s')
def AddModem(self, serial, index, sim):
    path = "/com/ubuntu/connectivity1/modem/{}".format(serial)
    self.AddObject(
        path,
        MODEM_IFACE,
        {
            'Index': dbus.Int32(index),
            'Serial': dbus.String(serial),
            'Sim': dbus.ObjectPath(sim)
        },
        [
        ]
    )
    return path


def add_openvpn_object(mock, path):
    obj = dbusmock.get_object(path)
    obj.AddProperties(VPN_CONN_OPENVPN_IFACE, {
        'connectionType': dbus.UInt32(0),
        'remote': dbus.String(),
        'ca': dbus.String(),
        'cert': dbus.String(),
        'certPass': dbus.String(),
        'key': dbus.String(),
        'username': dbus.String(),
        'password': dbus.String(),
        'localIp': dbus.String(),
        'remoteIp': dbus.String(),
        'staticKey': dbus.String(),
        'staticKeyDirection': dbus.UInt32(0),
        'portSet': dbus.Boolean(False),
        'port': dbus.UInt32(1194),
        'renegSecondsSet': dbus.Boolean(False),
        'renegSeconds': dbus.UInt32(0),
        'compLzo': dbus.Boolean(False),
        'protoTcp': dbus.Boolean(False),
        'devTypeSet': dbus.Boolean(False),
        'devType': dbus.UInt32(0),
        'dev': dbus.String(),
        'tunnelMtuSet': dbus.Boolean(False),
        'tunnelMtu': dbus.UInt32(1500),
        'fragmentSizeSet': dbus.Boolean(False),
        'fragmentSize': dbus.UInt32(1300),
        'mssFix': dbus.Boolean(False),
        'remoteRandom': dbus.Boolean(False),
        'cipher': dbus.UInt32(0),
        'keysizeSet': dbus.Boolean(False),
        'keysize': dbus.UInt32(128),
        'auth': dbus.UInt32(0),
        'tlsRemote': dbus.String(),
        'remoteCertTlsSet': dbus.Boolean(False),
        'remoteCertTls': dbus.UInt32(0),
        'taSet': dbus.Boolean(False),
        'ta': dbus.String(),
        'taDir': dbus.UInt32(0),
        'proxyType': dbus.UInt32(),
        'proxyServer': dbus.String(),
        'proxyPort': dbus.UInt32(80),
        'proxyRetry': dbus.Boolean(False),
        'proxyUsername': dbus.String(),
        'proxyPassword': dbus.String(),
    })


def add_pptp_object(mock, path):
    obj = dbusmock.get_object(path)
    obj.AddProperties(VPN_CONN_PPTP_IFACE, {
        'gateway': dbus.String(),
        'user': dbus.String(),
        'password': dbus.String(),
        'domain': dbus.String(),
        'allowPap': dbus.Boolean(True),
        'allowChap': dbus.Boolean(True),
        'allowMschap': dbus.Boolean(True),
        'allowMschapv2': dbus.Boolean(True),
        'allowEap': dbus.Boolean(True),
        'requireMppe': dbus.Boolean(False),
        'mppeType': dbus.UInt32(0),
        'mppeStateful': dbus.Boolean(False),
        'bsdCompression': dbus.Boolean(True),
        'deflateCompression': dbus.Boolean(True),
        'tcpHeaderCompression': dbus.Boolean(True),
        'sendPppEchoPackets': dbus.Boolean(False),
    })


def add_vpn_object(mock, vpn_type, path):
    mock.AddObject(
        path,
        VPN_CONN_IFACE,
        {
            'activatable': dbus.Boolean(True),
            'active': dbus.Boolean(False),
            'type': dbus.UInt32(vpn_type),
            'id': path.split('/')[len(path.split('/'))-1]
        },
        [
            ('UpdateSecrets', '', '', ''),
        ]
    )

    if vpn_type == 0:
        add_openvpn_object(mock, path)
    elif vpn_type == 1:
        add_pptp_object(mock, path)
    else:
        raise Exception("Unable to add vpn connection, no such type: %d" % (
            vpn_type)
        )


def add_vpn_connection(mock, vpn_type):
    conns = mock.Get(PRIV_IFACE, 'VpnConnections')
    new_path = '%s/%s%s' % (VPN_OBJ, 'MockVpnConnection', str(len(conns)))
    add_vpn_object(mock, vpn_type, new_path)
    conns.append(new_path)
    mock.SetProperty(PRIV_OBJ, PRIV_IFACE, 'VpnConnections', conns)
    return new_path


def remove_vpn_connection(mock, path):
    conns = mock.Get(PRIV_IFACE, 'VpnConnections')
    conns.remove(path)
    mock.SetProperty(PRIV_OBJ, PRIV_IFACE, 'VpnConnections', conns)


def load(mock, parameters):
    global _parameters
    _parameters = parameters

    mock.set_hotspot_enabled = set_hotspot_enabled
    mock.set_hotspot_ssid = set_hotspot_ssid
    mock.set_hotspot_password = set_hotspot_password
    mock.set_wifi_enabled = set_wifi_enabled
    mock.set_hotspot_auth = set_hotspot_auth
    mock.add_vpn_connection = add_vpn_connection
    mock.remove_vpn_connection = remove_vpn_connection

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
            'ModemAvailable': _parameters.get(
                'ModemAvailable', dbus.Boolean(True)
            ),
            'FlightModeSwitchEnabled': _parameters.get(
                'FlightModeSwitchEnabled', dbus.Boolean(False)
            ),
            'WifiSwitchEnabled': _parameters.get(
                'WifiSwitchEnabled', dbus.Boolean(False)
            ),
            'HotspotSwitchEnabled': _parameters.get(
                'HotspotSwitchEnabled', dbus.Boolean(False)
            ),
            'FlightMode': _parameters.get('FlightMode', dbus.Boolean(False)),
            'WifiEnabled': _parameters.get('WifiEnabled', dbus.Boolean(False)),
            # One of online, offline and connecting.
            'Status': _parameters.get('Status', 'offline')
        },
        []
    )

    mock.AddObject(
        PRIV_OBJ,
        PRIV_IFACE,
        {
            'HotspotPassword': _parameters.get(
                'HotspotPassword', dbus.String('abcdefgh')
            ),
            'HotspotAuth': _parameters.get(
                'HotspotAuth', dbus.String('wpa-psk')
            ),
            'VpnConnections': _parameters.get(
                'VpnConnections', dbus.Array([], signature='o')
            ),
            'MobileDataEnabled': _parameters.get(
                'MobileDataEnabled', dbus.Boolean(False)
            ),
            'SimForMobileData': _parameters.get(
                'SimForMobileData', dbus.ObjectPath('/')
            ),
            'Modems': _parameters.get(
                'Modems', dbus.Array([], signature='o')
            ),
            'Sims': _parameters.get(
                'Sims', dbus.Array([], signature='o')
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
                'objects["/"].set_wifi_enabled(self, args[0])'
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
                'SetHotspotAuth', 's', '',
                'objects["/"].set_hotspot_auth(self, args[0])'
            ),
            (
                'SetHotspotEnabled', 'b', '',
                'objects["/"].set_hotspot_enabled(self, args[0])'
            ),
            (
                'SetHotspotMode', 's', '',
                ''
            ),
            (
                'AddVpnConnection', 'u', 'o',
                'ret = objects["/"].add_vpn_connection(self, args[0])'
            ),
            (
                'RemoveVpnConnection', 'o', '',
                'objects["/"].remove_vpn_connection(self, args[0])'
            )
        ]
    )


@dbus.service.method(dbusmock.MOCK_IFACE,
                     in_signature='sssv', out_signature='')
def SetProperty(self, path, iface, name, value):
    obj = dbusmock.get_object(path)
    obj.Set(iface, name, value)
    obj.EmitSignal(iface, 'PropertiesChanged', 'a{sv}', [{name: value}])
