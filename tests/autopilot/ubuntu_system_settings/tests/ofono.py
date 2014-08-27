'''ofonod D-BUS mock template'''

# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU Lesser General Public License as published by the Free
# Software Foundation; either version 3 of the License, or (at your option) any
# later version.  See http://www.gnu.org/copyleft/lgpl.html for the full text
# of the license.

__author__ = 'Martin Pitt'
__email__ = 'martin.pitt@ubuntu.com'
__copyright__ = '(c) 2013 Canonical Ltd.'
__license__ = 'LGPL 3+'


import dbus

import dbusmock

BUS_NAME = 'org.ofono'
MAIN_OBJ = '/'
MAIN_IFACE = 'org.ofono.Manager'
SYSTEM_BUS = True

NOT_IMPLEMENTED = '''raise dbus.exceptions.DBusException(
    "org.ofono.Error.NotImplemented")'''

_parameters = {}


def load(mock, parameters):
    global _parameters
    mock.modems = []  # object paths
    _parameters = parameters
    mock.AddMethod(MAIN_IFACE, 'GetModems', '', 'a(oa{sv})',
                   'ret = [(m, objects[m].GetAll(\
                        "org.ofono.Modem")) for m in self.modems]')

    if not parameters.get('no_modem', False):
        mock.AddModem(parameters.get('ModemName', 'ril_0'), {})


@dbus.service.method(dbusmock.MOCK_IFACE,
                     in_signature='sa{sv}', out_signature='s')
def AddModem(self, name, properties):
    '''Convenience method to add a modem

    You have to specify a device name which must be a valid part of an object
    path, e. g. "mock_ac". For future extensions you can specify a "properties"
    array, but no extra properties are supported for now.

    Returns the new object path.
    '''
    path = '/' + name
    self.AddObject(
        path,
        'org.ofono.Modem',
        {
            'Online': dbus.Boolean(True, variant_level=1),
            'Powered': dbus.Boolean(True, variant_level=1),
            'Lockdown': dbus.Boolean(False, variant_level=1),
            'Emergency': dbus.Boolean(False, variant_level=1),
            'Manufacturer': dbus.String('Fakesys', variant_level=1),
            'Model': dbus.String('Mock Modem', variant_level=1),
            'Revision': dbus.String('0815.42', variant_level=1),
            'Type': dbus.String('hardware', variant_level=1),
            'Interfaces': [
                'org.ofono.CallVolume',
                'org.ofono.VoiceCallManager',
                'org.ofono.NetworkRegistration',
                'org.ofono.SimManager'],
            'Features': ['gprs', 'net'],
        },
        [
            ('GetProperties', '', 'a{sv}',
                'ret = self.GetAll("org.ofono.Modem")'),
            (
                'SetProperty', 'sv', '',
                'self.Set("org.ofono.Modem", args[0], args[1]); '
                'self.EmitSignal("org.ofono.Modem", "PropertyChanged",'
                '"sv", [args[0], args[1]])'),
        ])
    obj = dbusmock.mockobject.objects[path]
    obj.name = name
    add_simmanager_api(obj)
    add_voice_call_api(obj)
    add_netreg_api(obj)
    self.modems.append(path)
    props = obj.GetAll('org.ofono.Modem', dbus_interface=dbus.PROPERTIES_IFACE)
    self.EmitSignal(MAIN_IFACE, 'ModemAdded', 'oa{sv}', [path, props])
    return path


def add_simmanager_api(mock):
    '''Add org.ofono.SimManager API to a mock'''

    iface = 'org.ofono.SimManager'
    mock.AddProperties(iface, {
        'CardIdentifier': _parameters.get('CardIdentifier', 12345),
        'Present': _parameters.get('Present', dbus.Boolean(True)),
        'SubscriberNumbers': _parameters.get('SubscriberNumbers',
                                             ['123456789', '234567890']),
        'SubscriberIdentity': _parameters.get('SubscriberIdentity', 23456),
        'LockedPins': _parameters.get('LockedPins', ['pin']),
        'Retries': _parameters.get('Retries', {'pin': dbus.Byte(3)}),
        'PinRequired': _parameters.get('PinRequired', 'none')
    })

    mock.AddMethods(iface, [
        ('GetProperties', '', 'a{sv}', 'ret = self.GetAll("%s")' % iface),
        ('SetProperty', 'sv', '', 'self.Set("%(i)s", args[0], args[1]); '
         'self.EmitSignal("%(i)s", "PropertyChanged", "sv", [args[0], '
         'args[1]])' % {'i': iface}),
        ('ChangePin', 'sss', '', ''),
        ('EnterPin', 'ss', '', ''),
        ('ResetPin', 'sss', '', ''),
        ('LockPin', 'ss', '', 'if args[1] == "2468": self.Set("%(i)s",'
         '"LockedPins", dbus.Array(["pin"])); self.EmitSignal("%(i)s",'
         '"PropertyChanged", "sv", ["LockedPins", self.Get("%(i)s", '
         '"LockedPins")])' % {'i': iface}),
        ('UnlockPin', 'ss', '', 'if args[1] == "2468": self.Set("%(i)s",'
         '"LockedPins", ""); self.EmitSignal("%(i)s", "PropertyChanged", "sv",'
         ' ["LockedPins", self.Get("%(i)s", "LockedPins")])' % {'i': iface})
    ])


def add_voice_call_api(mock):
    '''Add org.ofono.VoiceCallManager API to a mock'''

    # also add an emergency number which is not a real one, in case one runs a
    # test case against a production ofono :-)
    mock.AddProperty(
        'org.ofono.VoiceCallManager', 'EmergencyNumbers', ['911', '13373'])

    mock.calls = []  # object paths

    mock.AddMethods('org.ofono.VoiceCallManager', [
        (
            'GetProperties', '', 'a{sv}',
            'ret = self.GetAll("org.ofono.VoiceCallManager")'),
        ('Transfer', '', '', ''),
        ('SwapCalls', '', '', ''),
        ('ReleaseAndAnswer', '', '', ''),
        ('ReleaseAndSwap', '', '', ''),
        ('HoldAndAnswer', '', '', ''),
        ('SendTones', 's', '', ''),
        ('PrivateChat', 'o', 'ao', NOT_IMPLEMENTED),
        ('CreateMultiparty', '', 'o', NOT_IMPLEMENTED),
        ('HangupMultiparty', '', '', NOT_IMPLEMENTED),
        (
            'GetCalls', '', 'a(oa{sv})',
            'ret = [(c, objects[c].GetAll("org.ofono.VoiceCall"))\
             for c in self.calls]')
    ])


@dbus.service.method('org.ofono.VoiceCallManager',
                     in_signature='ss', out_signature='s')
def Dial(self, number, hide_callerid):
    path = self._object_path + '/voicecall%02i' % (len(self.calls) + 1)
    self.AddObject(
        path, 'org.ofono.VoiceCall',
        {
            'State': dbus.String('dialing', variant_level=1),
            'LineIdentification': dbus.String(number, variant_level=1),
            'Name': dbus.String('', variant_level=1),
            'Multiparty': dbus.Boolean(False, variant_level=1),
            'Multiparty': dbus.Boolean(False, variant_level=1),
            'RemoteHeld': dbus.Boolean(False, variant_level=1),
            'RemoteMultiparty': dbus.Boolean(False, variant_level=1),
            'Emergency': dbus.Boolean(False, variant_level=1),
        },
        [
            (
                'GetProperties', '', 'a{sv}',
                'ret = self.GetAll("org.ofono.VoiceCall")'),
            ('Deflect', 's', '', NOT_IMPLEMENTED),
            (
                'Hangup', '', '',
                'self.parent.calls.remove(self._object_path);'
                'self.parent.RemoveObject(self._object_path);'
                'self.EmitSignal("org.ofono.VoiceCallManager",\
                     "CallRemoved", "o", [self._object_path])'),
            ('Answer', '', '', NOT_IMPLEMENTED),
        ])
    obj = dbusmock.mockobject.objects[path]
    obj.parent = self
    self.calls.append(path)
    self.EmitSignal('org.ofono.VoiceCallManager', 'CallAdded', 'oa{sv}',
                    [path, obj.GetProperties()])
    return path


@dbus.service.method('org.ofono.VoiceCallManager',
                     in_signature='', out_signature='')
def HangupAll(self):
    print('XXX HangupAll', self.calls)
    for c in list(self.calls):  # needs a copy
        dbusmock.mockobject.objects[c].Hangup()
    assert self.calls == []


def get_all_operators(mock):
    return 'ret = [(m, objects[m].GetAll("org.ofono.NetworkOperator")) ' \
           'for m in objects if "%s/operator/" in m]' % mock.name


def add_netreg_api(mock):
    '''Add org.ofono.NetworkRegistration API to a mock'''

    # also add an emergency number which is not a real one, in case one runs a
    # test case against a production ofono :-)
    mock.AddProperties('org.ofono.NetworkRegistration', {
        'Mode': 'auto',
        'Status': 'registered',
        'LocationAreaCode': _parameters.get('LocationAreaCode', 987),
        'CellId': _parameters.get('CellId', 10203),
        'MobileCountryCode': _parameters.get('MobileCountryCode', '777'),
        'MobileNetworkCode': _parameters.get('MobileNetworkCode', '11'),
        'Technology': _parameters.get('Technology', 'gsm'),
        'Name': _parameters.get('Name', 'fake.tel'),
        'Strength': _parameters.get('Strength', dbus.Byte(80)),
        'BaseStation': _parameters.get('BaseStation', ''),
    })

    mock.AddObject(
        '/%s/operator/op1' % mock.name,
        'org.ofono.NetworkOperator',
        {
            'Name': _parameters.get('Name', 'fake.tel'),
            'Status': 'current',
            'MobileCountryCode': _parameters.get('MobileCountryCode', '777'),
            'MobileNetworkCode': _parameters.get('MobileNetworkCode', '11'),
            'Technologies': [_parameters.get('Technology', 'gsm')],
        },
        [
            (
                'GetProperties', '', 'a{sv}',
                'ret = self.GetAll("org.ofono.NetworkOperator")'),
            ('Register', '', '', ''),
        ])

    mock.AddMethods('org.ofono.NetworkRegistration', [
        (
            'GetProperties', '', 'a{sv}',
            'ret = self.GetAll("org.ofono.NetworkRegistration")'),
        ('Register', '', '', ''),
        ('GetOperators', '', 'a(oa{sv})', get_all_operators(mock)),
        ('Scan', '', 'a(oa{sv})', get_all_operators(mock)),
    ])
