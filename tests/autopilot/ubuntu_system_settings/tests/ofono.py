'''ofonod D-BUS mock template'''

# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU Lesser General Public License as published by the Free
# Software Foundation; either version 3 of the License, or (at your option) any
# later version.  See http://www.gnu.org/copyleft/lgpl.html for the full text
# of the license.
import dbus
import dbusmock

__author__ = 'Martin Pitt'
__email__ = 'martin.pitt@ubuntu.com'
__copyright__ = '(c) 2013 Canonical Ltd.'
__license__ = 'LGPL 3+'

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
                'org.ofono.CallForwarding',
                'org.ofono.CallSettings',
                'org.ofono.CallVolume',
                'org.ofono.ConnectionManager',
                'org.ofono.NetworkRegistration',
                'org.ofono.RadioSettings',
                'org.ofono.SimManager',
                'org.ofono.VoiceCallManager',
                ],
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
    obj.sim_pin = "2468"
    add_simmanager_api(obj)
    add_connectionmanager_api(obj)
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
        'ServiceNumbers': _parameters.get('ServiceNumbers', dbus.Dictionary(
            {
                'Fake Service': dbus.String('555-555', variant_level=1),
                'Faker Service': dbus.String('555-321', variant_level=1),
                'Fakest Service': dbus.String('555-123', variant_level=1)
            }, signature='sv'
        )),
        'LockedPins': _parameters.get('LockedPins', ['pin']),
        'Retries': _parameters.get('Retries', {'pin': dbus.Byte(3)}),
        'PinRequired': _parameters.get('PinRequired', 'none')
    })

    mock.AddMethods(iface, [
        ('GetProperties', '', 'a{sv}', 'ret = self.GetAll("%s")' % iface),
        ('SetProperty', 'sv', '', 'self.Set("%(i)s", args[0], args[1]); '
         'self.EmitSignal("%(i)s", "PropertyChanged", "sv", [args[0], '
         'args[1]])' % {'i': iface}),
        ('ResetPin', 'sss', '', '')
    ])


def add_connectionmanager_api(mock):
    '''Add org.ofono.ConnectionManager API to a mock'''

    iface = 'org.ofono.ConnectionManager'
    mock.contexts = []
    mock.AddProperties(iface, {
        'Attached': _parameters.get('Attached', True),
        'Bearer': _parameters.get('Bearer', 'gprs'),
        'RoamingAllowed': _parameters.get('RoamingAllowed', False),
        'Powered': _parameters.get('ConnectionPowered', True),
    })
    mock.AddMethods(iface, [
        ('GetProperties', '', 'a{sv}', 'ret = self.GetAll("%s")' % iface),
        ('SetProperty', 'sv', '', 'self.Set("%(i)s", args[0], args[1]); '
         'self.EmitSignal("%(i)s", "PropertyChanged", "sv", ['
         'args[0], args[1]])' % {'i': iface}),
        ('AddContext', 's', 'o', 'ret = self.AddConnectionContext(args[0])'),
        ('RemoveContext', 'o', '', 'self.RemoveConnectionContext(args[0])'),
        ('DeactivateAll', '', '', ''),
        ('GetContexts', '', 'a(oa{sv})', 'ret = self.GetConnectionContexts()'),
    ])

    interfaces = mock.GetProperties()['Interfaces']
    interfaces.append(iface)
    mock.SetProperty('Interfaces', interfaces)


@dbus.service.method('org.ofono.ConnectionManager',
                     in_signature='', out_signature='a(oa{sv})')
def GetConnectionContexts(self):
    contexts = dbus.Array([], signature='a(oa{sv})')
    for ctx in self.contexts:
        contexts.append(dbus.Struct(
            (ctx.__dbus_object_path__, ctx.GetProperties())))
    return contexts


@dbus.service.method('org.ofono.ConnectionManager',
                     in_signature='s', out_signature='o')
def AddConnectionContext(self, type):
    name = 'context%s' % str(len(self.contexts))
    path = '%s/%s' % (self.__dbus_object_path__, name)
    iface = 'org.ofono.ConnectionContext'

    # We give the context a name, just like ofono does.
    # See https://github.com/rilmodem/ofono/blob/master/src/gprs.c#L148
    ofono_default_accesspointname = {
        'internet': 'Internet',
        'mms': 'MMS',
        'ai': 'AI',
        'ims': 'IMS',
        'wap': 'WAP'
    }

    self.AddObject(
        path,
        iface,
        {
            'Active': False,
            'AccessPointName': '',
            'Type': type,
            'Username': '',
            'Password': '',
            'Protocol': 'ip',
            'Name': ofono_default_accesspointname[type],
            'Preferred': False,
            'Settings': dbus.Dictionary({}, signature='sv'),
            'IPv6.Settings': dbus.Dictionary({}, signature='sv'),
            'MessageProxy': '',
            'MessageCenter': '',
        },
        [
            ('GetProperties', '', 'a{sv}',
                'ret = self.GetAll("org.ofono.ConnectionContext")'),
            (
                'SetProperty', 'sv', '',
                'self.Set("%s", args[0], args[1]); '
                'self.EmitSignal("%s", "PropertyChanged",'
                '"sv", [args[0], args[1]])' % (iface, iface)),
        ])
    ctx_obj = dbusmock.get_object(path)
    self.contexts.append(ctx_obj)
    self.EmitSignal('org.ofono.ConnectionManager',
                    'ContextAdded', 'oa{sv}', [path, ctx_obj.GetProperties()])

    return path


@dbus.service.method('org.ofono.ConnectionManager',
                     in_signature='o', out_signature='')
def RemoveConnectionContext(self, path):
    ctx_obj = dbusmock.get_object(path)
    self.contexts.remove(ctx_obj)
    self.RemoveObject(path)
    self.EmitSignal('org.ofono.ConnectionManager',
                    'ContextRemoved', 'o', [path])


@dbus.service.method('org.ofono.SimManager',
                     in_signature='ss', out_signature='')
def LockPin(self, pin_type, pin):
    iface = 'org.ofono.SimManager'
    print('XXX LockPin', pin_type, pin)

    if (pin == self.sim_pin):
        print('XXX LockPin pin matches')
        self.Set(iface, "LockedPins", dbus.Array(["pin"]))
        self.EmitSignal(iface, "PropertyChanged", "sv",
                        ["LockedPins", self.Get(iface, "LockedPins")])
        self.Set(iface, "Retries", {'pin': dbus.Byte(3)})
        self.EmitSignal(iface, "PropertyChanged", "sv",
                        ["Retries", self.Get(iface, "Retries")])
    else:
        retries = self.Get(iface, "Retries")['pin']
        if (retries > 0):
            self.Set(iface, "Retries", {'pin': dbus.Byte(retries - 1)})
            self.EmitSignal(iface, "PropertyChanged", "sv",
                            ["Retries", self.Get(iface, "Retries")])
        raise dbus.exceptions.DBusException("", "Failed",
                                            name="org.ofono.Error.Failed")
    print('XXX LockPin', self.Get(iface, "Retries")['pin'])


@dbus.service.method('org.ofono.SimManager',
                     in_signature='ss', out_signature='')
def UnlockPin(self, pin_type, pin):
    iface = 'org.ofono.SimManager'
    print('XXX UnlockPin', pin_type, pin)

    if (pin == self.sim_pin):
        print('XXX UnlockPin pin matches')
        self.Set(iface, "LockedPins", "")
        self.EmitSignal(iface, "PropertyChanged", "sv",
                        ["LockedPins", self.Get(iface, "LockedPins")])
        self.Set(iface, "Retries", {'pin': dbus.Byte(3)})
        self.EmitSignal(iface, "PropertyChanged", "sv",
                        ["Retries", self.Get(iface, "Retries")])
    else:
        retries = self.Get(iface, "Retries")['pin']
        if (retries > 0):
            self.Set(iface, "Retries", {'pin': dbus.Byte(retries - 1)})
            self.EmitSignal(iface, "PropertyChanged", "sv",
                            ["Retries", self.Get(iface, "Retries")])
        raise dbus.exceptions.DBusException("", "Failed",
                                            name="org.ofono.Error.Failed")
    print('XXX UnlockPin', self.Get(iface, "Retries")['pin'])


@dbus.service.method('org.ofono.SimManager',
                     in_signature='sss', out_signature='')
def ChangePin(self, pin_type, pin, pin2):
    iface = 'org.ofono.SimManager'
    print('XXX ChangePin', pin_type, pin, pin2)

    if (pin == self.sim_pin):
        print('XXX ChangePin pin matches')
        self.sim_pin = pin2
        self.Set(iface, "Retries", {'pin': dbus.Byte(3)})
        self.EmitSignal(iface, "PropertyChanged", "sv",
                        ["Retries", self.Get(iface, "Retries")])
    else:
        retries = self.Get(iface, "Retries")['pin']
        if (retries > 0):
            self.Set(iface, "Retries", {'pin': dbus.Byte(retries - 1)})
            self.EmitSignal(iface, "PropertyChanged", "sv",
                            ["Retries", self.Get(iface, "Retries")])
        raise dbus.exceptions.DBusException("", "Failed",
                                            name="org.ofono.Error.Failed")


@dbus.service.method('org.ofono.SimManager',
                     in_signature='ss', out_signature='')
def EnterPin(self, pin_type, pin):
    iface = 'org.ofono.SimManager'
    print('XXX EnterPin', pin)

    if (pin == self.sim_pin):
        print('XXX EnterPin pin matches')
        self.Set(iface, "Retries", {'pin': dbus.Byte(3)})
        self.EmitSignal(iface, "PropertyChanged", "sv",
                        ["Retries", self.Get(iface, "Retries")])
    else:
        retries = self.Get(iface, "Retries")['pin']
        if (retries > 0):
            self.Set(iface, "Retries", {'pin': dbus.Byte(retries - 1)})
            self.EmitSignal(iface, "PropertyChanged", "sv",
                            ["Retries", self.Get(iface, "Retries")])
        raise dbus.exceptions.DBusException("", "Failed",
                                            name="org.ofono.Error.Failed")


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
