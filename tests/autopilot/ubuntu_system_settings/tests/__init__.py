# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2013 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

""" Tests for Ubuntu System Settings """
from __future__ import absolute_import


import dbus
import dbusmock
import os
import subprocess
import ubuntuuitoolkit

from autopilot.matchers import Eventually
from dbusmock.templates.networkmanager import DEVICE_IFACE
from datetime import datetime
from fixtures import EnvironmentVariable
from testtools.matchers import Equals, NotEquals, GreaterThan
from time import sleep
from ubuntu_system_settings import SystemSettings
from gi.repository import UPowerGlib

ACCOUNTS_IFACE = 'org.freedesktop.Accounts'
ACCOUNTS_USER_IFACE = 'org.freedesktop.Accounts.User'
ACCOUNTS_OBJ = '/org/freedesktop/Accounts'
ACCOUNTS_SERVICE = 'com.canonical.unity.AccountsService'
ACCOUNTS_SOUND_IFACE = 'com.ubuntu.touch.AccountsService.Sound'
ISOUND_SERVICE = 'com.canonical.indicator.sound'
ISOUND_ACTION_PATH = '/com/canonical/indicator/sound'
GTK_ACTIONS_IFACE = 'org.gtk.Actions'
MODEM_IFACE = 'org.ofono.Modem'
CONNMAN_IFACE = 'org.ofono.ConnectionManager'
RDO_IFACE = 'org.ofono.RadioSettings'
SIM_IFACE = 'org.ofono.SimManager'
NETREG_IFACE = 'org.ofono.NetworkRegistration'
NETOP_IFACE = 'org.ofono.NetworkOperator'
CALL_FWD_IFACE = 'org.ofono.CallForwarding'
CALL_SETTINGS_IFACE = 'org.ofono.CallSettings'
SYSTEM_IFACE = 'com.canonical.SystemImage'
SYSTEM_SERVICE_OBJ = '/Service'
LM_SERVICE = 'org.freedesktop.login1'
LM_PATH = '/org/freedesktop/login1'
LM_IFACE = 'org.freedesktop.login1.Manager'
NM_SERVICE = 'org.freedesktop.NetworkManager'
NM_PATH = '/org/freedesktop/NetworkManager'
NM_IFACE = 'org.freedesktop.NetworkManager'
UPOWER_VERSION = str(UPowerGlib.MAJOR_VERSION)
UPOWER_VERSION += '.' + str(UPowerGlib.MINOR_VERSION)


class UbuntuSystemSettingsTestCase(
        ubuntuuitoolkit.base.UbuntuUIToolkitAppTestCase):

    """Base class for Ubuntu System Settings."""

    def setUp(self, panel=None):
        super(UbuntuSystemSettingsTestCase, self).setUp()
        self.system_settings = SystemSettings(self, panel=panel)
        self.assertThat(
            self.system_settings.main_view.visible,
            Eventually(Equals(True)))

    def set_orientation(self, gsettings, value):
        gsettings.set_value('rotation-lock', value)
        self.assertThat(
            lambda: gsettings.get_value('rotation-lock').get_boolean(),
            Eventually(Equals(value.get_boolean())))


class UbuntuSystemSettingsUpowerTestCase(UbuntuSystemSettingsTestCase,
                                         dbusmock.DBusTestCase):
    """Base class for battery tests that mocks Upower"""

    @classmethod
    def setUpClass(cls):
        cls.start_system_bus()
        cls.dbus_con = cls.get_dbus(True)
        # Add a mock Upower environment so we get consistent results
        (cls.p_mock, cls.obj_upower) = cls.spawn_server_template(
            'upower',
            {'OnBattery': True, 'DaemonVersion': UPOWER_VERSION},
            stdout=subprocess.PIPE)
        cls.dbusmock = dbus.Interface(cls.obj_upower, dbusmock.MOCK_IFACE)

    def setUp(self, panel=None):
        self.obj_upower.Reset()
        super(UbuntuSystemSettingsUpowerTestCase, self).setUp()

    def add_mock_battery(self):
        """ Make sure we have a battery """
        self.dbusmock.AddDischargingBattery(
            'mock_BATTERY', 'Battery', 50.0, 10
        )

    @classmethod
    def tearDownClass(cls):
        cls.p_mock.terminate()
        cls.p_mock.wait()
        super(UbuntuSystemSettingsUpowerTestCase, cls).tearDownClass()


class UbuntuSystemSettingsBatteryTestCase(UbuntuSystemSettingsUpowerTestCase):
    """ Base class for tests which rely on the presence of a battery """

    def setUp(self):
        super(UbuntuSystemSettingsBatteryTestCase, self).setUp()
        self.add_mock_battery()
        self.system_settings = SystemSettings(self)
        self.assertThat(
            self.system_settings.main_view.visible,
            Eventually(Equals(True)))


class UbuntuSystemSettingsOfonoTestCase(UbuntuSystemSettingsTestCase,
                                        dbusmock.DBusTestCase):
    """Class for cellular tests which sets up an Ofono mock """

    use_sims = 1

    @property
    def choose_carrier_page(self):
        """Return carrier selection page"""
        return self.system_settings.main_view.select_single(
            objectName='chooseCarrierPage'
        )

    @property
    def choose_carriers_page(self):
        """Return carriers selection page"""
        return self.system_settings.main_view.select_single(
            objectName='chooseCarriersPage'
        )

    # TODO: remove this when it has been fixed in dbusmock
    def get_all_operators(self, name):
        return 'ret = [(m, objects[m].GetAll("org.ofono.NetworkOperator")) ' \
               'for m in objects if "%s/operator/" in m]' % name

    def mock_connection_manager(self, modem):
        modem.AddProperty(CONNMAN_IFACE, 'Powered', dbus.Boolean(1))
        modem.AddProperty(CONNMAN_IFACE, 'RoamingAllowed', False)
        modem.AddMethods(
            CONNMAN_IFACE,
            [
                (
                    'GetProperties', '', 'a{sv}',
                    'ret = self.GetAll("%s")' % CONNMAN_IFACE),
                (
                    'SetProperty', 'sv', '',
                    'self.Set("IFACE", args[0], args[1]); '
                    'self.EmitSignal("IFACE", "PropertyChanged", "sv",\
                        [args[0], args[1]])'.replace("IFACE", CONNMAN_IFACE)),
            ])

    def mock_carriers(self, name):
        self.dbusmock.AddObject(
            '/%s/operator/op2' % name,
            NETOP_IFACE,
            {
                'Name': 'my.cool.telco',
                'Status': 'available',
                'MobileCountryCode': '777',
                'MobileNetworkCode': '22',
                'Technologies': ['gsm'],
            },
            [
                ('GetProperties', '', 'a{sv}',
                    'ret = self.GetAll("org.ofono.NetworkOperator")'),
                ('Register', '', '', ''),
            ]
        )
        # Add a forbidden carrier
        self.dbusmock.AddObject(
            '/%s/operator/op3' % name,
            NETOP_IFACE,
            {
                'Name': 'my.bad.telco',
                'Status': 'forbidden',
                'MobileCountryCode': '777',
                'MobileNetworkCode': '22',
                'Technologies': ['gsm'],
            },
            [
                ('GetProperties', '', 'a{sv}',
                    'ret = self.GetAll("org.ofono.NetworkOperator")'),
                ('Register', '', '', ''),
            ]
        )

    def mock_radio_settings(self, modem, preference='gsm',
                            technologies=['gsm', 'umts', 'lte']):
        modem.AddProperty(
            RDO_IFACE, 'TechnologyPreference', preference)
        modem.AddProperty(
            RDO_IFACE, 'ModemTechnologies', technologies)
        modem.AddMethods(
            RDO_IFACE,
            [('GetProperties', '', 'a{sv}',
              'ret = self.GetAll("%s")' % RDO_IFACE),
                ('SetProperty', 'sv', '',
                 'self.Set("IFACE", args[0], args[1]); '
                 'self.EmitSignal("IFACE",\
                 "PropertyChanged", "sv", [args[0], args[1]])'
                    .replace('IFACE', RDO_IFACE)), ])

    def mock_call_forwarding(self, modem):
        modem.AddProperty(
            CALL_FWD_IFACE, 'VoiceUnconditional', '')
        modem.AddMethods(
            CALL_FWD_IFACE,
            [('GetProperties', '', 'a{sv}',
              'ret = self.GetAll("%s")' % CALL_FWD_IFACE),
                ('SetProperty', 'sv', '',
                 'self.Set("IFACE", args[0], args[1]); '
                 'self.EmitSignal("IFACE",\
                 "PropertyChanged", "sv", [args[0], args[1]])'
                    .replace('IFACE', CALL_FWD_IFACE)), ])

    def mock_call_settings(self, modem):
        modem.AddProperty(
            CALL_SETTINGS_IFACE, 'VoiceCallWaiting', 'disabled')
        modem.AddMethods(
            CALL_SETTINGS_IFACE,
            [('GetProperties', '', 'a{sv}',
              'ret = self.GetAll("%s")' % CALL_SETTINGS_IFACE),
                ('SetProperty', 'sv', '',
                 'self.Set("IFACE", args[0], args[1]); '
                 'self.EmitSignal("IFACE",\
                 "PropertyChanged", "sv", [args[0], args[1]])'
                    .replace('IFACE', CALL_SETTINGS_IFACE)), ])

    def add_sim1(self):
        # create modem_0 proxy
        self.modem_0 = self.dbus_con.get_object('org.ofono', '/ril_0')

        self.mock_carriers('ril_0')
        self.mock_radio_settings(self.modem_0)
        self.mock_connection_manager(self.modem_0)
        self.mock_call_forwarding(self.modem_0)
        self.mock_call_settings(self.modem_0)

        self.modem_0.AddMethods('org.ofono.NetworkRegistration', [
            ('GetProperties', '', 'a{sv}',
                'ret = self.GetAll("org.ofono.NetworkRegistration")'),
            ('Register', '', '', ''),
            ('GetOperators', '', 'a(oa{sv})', self.get_all_operators('ril_0')),
            ('Scan', '', 'a(oa{sv})', self.get_all_operators('ril_0')),
        ])

    def add_sim2(self):
        '''Mock two modems/sims for the dual sim story'''
        second_modem = 'ril_1'

        self.dbusmock.AddModem(second_modem, {'Powered': True})
        self.modem_1 = self.dbus_con.get_object(
            'org.ofono', '/%s' % second_modem)

        self.modem_1.AddMethods(NETREG_IFACE, [
            ('GetProperties', '', 'a{sv}',
                'ret = self.GetAll("org.ofono.NetworkRegistration")'),
            ('Register', '', '', ''),
            ('GetOperators', '', 'a(oa{sv})',
                self.get_all_operators(second_modem)),
            ('Scan', '', 'a(oa{sv})',
                self.get_all_operators(second_modem)),
        ])
        self.mock_carriers(second_modem)
        self.mock_radio_settings(self.modem_1, technologies=['gsm'])
        self.mock_connection_manager(self.modem_1)
        self.mock_call_forwarding(self.modem_1)
        self.mock_call_settings(self.modem_1)

        self.modem_1.Set(
            SIM_IFACE,
            'SubscriberNumbers', ['08123', '938762783']
        )

    @classmethod
    def setUpClass(cls):
        cls.start_system_bus()
        cls.dbus_con = cls.get_dbus(True)
        template = os.path.join(os.path.dirname(__file__), 'ofono.py')
        # Add a mock Ofono environment so we get consistent results
        (cls.p_mock, cls.obj_ofono) = cls.spawn_server_template(
            template, stdout=subprocess.PIPE)
        cls.dbusmock = dbus.Interface(cls.obj_ofono, dbusmock.MOCK_IFACE)

    def setUp(self, panel=None):
        self.obj_ofono.Reset()

        self.add_sim1()
        if self.use_sims == 2:
            self.add_sim2()

        super(UbuntuSystemSettingsOfonoTestCase, self).setUp(panel)

    def set_default_for_calls(self, gsettings, default):
        gsettings.set_value('default-sim-for-calls', default)
        # wait for gsettings
        sleep(1)

    def set_default_for_messages(self, gsettings, default):
        gsettings.set_value('default-sim-for-messages', default)
        # wait for gsettings
        sleep(1)

    def get_default_sim_for_calls_selector(self, text):
        return self.system_settings.main_view.cellular_page.select_single(
            objectName="defaultForCalls" + text
        )

    def get_default_sim_for_messages_selector(self, text):
        return self.system_settings.main_view.cellular_page.select_single(
            objectName="defaultForMessages" + text
        )


class CellularBaseTestCase(UbuntuSystemSettingsOfonoTestCase):

    def setUp(self):
        """ Go to Cellular page """
        super(CellularBaseTestCase, self).setUp()
        self.cellular_page = self.system_settings.\
            main_view.go_to_cellular_page()


class PhoneOfonoBaseTestCase(UbuntuSystemSettingsOfonoTestCase):
    def setUp(self):
        """ Go to Phone page """
        super(PhoneOfonoBaseTestCase, self).setUp('phone')
        self.phone_page = self.system_settings.main_view.select_single(
            objectName='phonePage'
        )


class AboutBaseTestCase(UbuntuSystemSettingsTestCase):
    def setUp(self):
        """Go to About page."""
        super(AboutBaseTestCase, self).setUp('about')
        self.about_page = self.system_settings.main_view.about_page


class AboutOfonoBaseTestCase(UbuntuSystemSettingsOfonoTestCase):
    def setUp(self):
        """Go to About page."""
        super(AboutOfonoBaseTestCase, self).setUp('about')
        self.about_page = self.system_settings.main_view.about_page


class AboutSystemImageBaseTestCase(AboutBaseTestCase,
                                   dbusmock.DBusTestCase):
    @classmethod
    def setUpClass(cls):
        cls.start_system_bus()
        cls.dbus_con = cls.get_dbus(True)
        cls.p_mock = cls.spawn_server('com.canonical.SystemImage',
                                      '/Service',
                                      'com.canonical.SystemImage',
                                      system_bus=True,
                                      stdout=subprocess.PIPE)
        cls.dbusmock = dbus.Interface(cls.dbus_con.get_object(
                                      'com.canonical.SystemImage',
                                      '/Service'),
                                      dbusmock.MOCK_IFACE)

        date = datetime.now().replace(microsecond=0).isoformat()

        cls.dbusmock.AddMethod('', 'Info', '', 'isssa{ss}',
                               'ret = (0, "", "", "%s", [])' % date)

    def setUp(self):
        self.wait_for_bus_object('com.canonical.SystemImage',
                                 '/Service',
                                 system_bus=True)
        super(AboutSystemImageBaseTestCase, self).setUp()

    @classmethod
    def tearDownClass(cls):
        cls.p_mock.terminate()
        cls.p_mock.wait()
        super(AboutSystemImageBaseTestCase, cls).tearDownClass()


class StorageBaseTestCase(AboutBaseTestCase):

    """Base class for Storage page tests."""

    def setUp(self):
        """Go to Storage Page."""
        super(StorageBaseTestCase, self).setUp()
        self.system_settings.main_view.click_item('storageItem')
        self.assertThat(self.storage_page.active, Eventually(Equals(True)))

    def assert_space_item(self, object_name, text):
        """ Checks whether an space item exists and returns a value """
        item = self.system_settings.main_view.storage_page.select_single(
            objectName=object_name
        )
        self.assertThat(item, NotEquals(None))
        label = item.label  # Label
        self.assertThat(label, Equals(text))
        # Get item's label
        size_label = item.select_single(objectName='sizeLabel')
        self.assertThat(size_label, NotEquals(None))
        values = size_label.text.split(' ')  # Format: "00.0 (bytes|MB|GB)"
        self.assertThat(len(values), GreaterThan(1))

    def get_storage_space_used_by_category(self, objectName):
        return self.main_view.wait_select_single(
            'StorageItem', objectName=objectName).value

    @property
    def storage_page(self):
        """ Return 'Storage' page """
        return self.system_settings.main_view.select_single(
            'Storage', objectName='storagePage'
        )


class LicenseBaseTestCase(AboutBaseTestCase):

    """Base class for Licenses page tests."""

    def setUp(self):
        """Go to License Page."""
        super(LicenseBaseTestCase, self).setUp()
        self.licenses_page = self.about_page.go_to_software_licenses()


class SystemUpdatesBaseTestCase(UbuntuSystemSettingsTestCase):

    """Base class for SystemUpdates page tests."""

    def setUp(self):
        """Go to SystemUpdates Page."""
        super(SystemUpdatesBaseTestCase, self).setUp()
        self.system_settings.main_view.click_item(
            'entryComponent-system-update')


class BackgroundBaseTestCase(
        UbuntuSystemSettingsTestCase,
        dbusmock.DBusTestCase):
    """ Base class for Background tests """

    @classmethod
    def setUpClass(klass):
        klass.start_system_bus()
        klass.dbus_con = klass.get_dbus(True)

    # TODO: create dbusmock template
    def setUp(self):
        """Mock account service dbus, go to background page"""

        # mock ubuntu art directory using a local path
        art_dir = '%s/../background_images/' % (
            os.path.dirname(os.path.realpath(__file__)))
        user_obj = '/user/foo'

        self.user_props = {
            'BackgroundFile': dbus.String(
                '%slaunchpad.jpg' % art_dir, variant_level=1)
        }

        # start dbus system bus
        self.mock_server = self.spawn_server(ACCOUNTS_IFACE, ACCOUNTS_OBJ,
                                             ACCOUNTS_IFACE, system_bus=True,
                                             stdout=subprocess.PIPE)

        sleep(2)

        # create account proxy
        self.acc_proxy = dbus.Interface(self.dbus_con.get_object(
            ACCOUNTS_IFACE, ACCOUNTS_OBJ), dbusmock.MOCK_IFACE)

        # let accountservice find a user object path
        self.acc_proxy.AddMethod(ACCOUNTS_IFACE, 'FindUserById', 'x', 'o',
                                 'ret = "%s"' % user_obj)

        # add getter and setter to mock
        self.acc_proxy.AddMethods(
            'org.freedesktop.DBus.Properties',
            [('Get', 's', 'v', 'ret = self.user_props[args[0]]'),
                ('Set', 'sv', '', 'self.user_props[args[0]] = args[1]')])

        # add user object to mock
        self.acc_proxy.AddObject(
            user_obj, ACCOUNTS_USER_IFACE, self.user_props,
            [
                (
                    'SetBackgroundFile', 'v', '',
                    'self.Set("%s", "BackgroundFile", args[0]);' %
                    ACCOUNTS_USER_IFACE),
                (
                    'GetBackgroundFile', '', 'v',
                    'ret = self.Get("%s", "BackgroundFile")' %
                    ACCOUNTS_USER_IFACE)
            ])

        # create user proxy
        self.user_proxy = dbus.Interface(self.dbus_con.get_object(
            ACCOUNTS_IFACE, user_obj),
            ACCOUNTS_USER_IFACE)

        # patch env variable
        self.useFixture(EnvironmentVariable(
            'SYSTEM_SETTINGS_UBUNTU_ART_DIR', art_dir))

        super(BackgroundBaseTestCase, self).setUp('background')
        self.assertThat(self.system_settings.main_view.background_page.active,
                        Eventually(Equals(True)))

    def tearDown(self):
        self.mock_server.terminate()
        self.mock_server.wait()
        super(BackgroundBaseTestCase, self).tearDown()


class SoundBaseTestCase(
        UbuntuSystemSettingsTestCase,
        dbusmock.DBusTestCase):
    """ Base class for sound settings tests"""

    @classmethod
    def setUpClass(klass):
        klass.start_system_bus()
        klass.start_session_bus()
        klass.dbus_con = klass.get_dbus(True)
        klass.dbus_con_session = klass.get_dbus(False)

    def setUp(self, panel='sound'):

        user_obj = '/user/foo'

        self.accts_snd_props = {
            'IncomingCallVibrate': dbus.Boolean(False, variant_level=1),
            'IncomingCallVibrateSilentMode': dbus.Boolean(False,
                                                          variant_level=1),
            'IncomingMessageVibrate': dbus.Boolean(False,
                                                   variant_level=1),
            'IncomingMessageVibrateSilentMode': dbus.Boolean(False,
                                                             variant_level=1),
            'DialpadSoundsEnabled': dbus.Boolean(True,
                                                 variant_level=1)}

        # start dbus system bus
        self.mock_server = self.spawn_server(ACCOUNTS_IFACE, ACCOUNTS_OBJ,
                                             ACCOUNTS_IFACE, system_bus=True,
                                             stdout=subprocess.PIPE)

        # start isound
        self.mock_isound = self.spawn_server(ISOUND_SERVICE,
                                             ISOUND_ACTION_PATH,
                                             GTK_ACTIONS_IFACE,
                                             stdout=subprocess.PIPE)

        sleep(2)

        self.dbus_mock = dbus.Interface(self.dbus_con.get_object(
                                        ACCOUNTS_IFACE,
                                        ACCOUNTS_OBJ,
                                        ACCOUNTS_IFACE),
                                        dbusmock.MOCK_IFACE)

        self.dbus_mock_isound = dbus.Interface(
            self.dbus_con_session.get_object(
                ISOUND_SERVICE,
                ISOUND_ACTION_PATH,
                GTK_ACTIONS_IFACE),
            dbusmock.MOCK_IFACE)

        # let accountservice find a user object path
        self.dbus_mock.AddMethod(ACCOUNTS_IFACE, 'FindUserById', 'x', 'o',
                                 'ret = "%s"' % user_obj)

        self.dbus_mock.AddProperties(ACCOUNTS_SOUND_IFACE,
                                     self.accts_snd_props)

        # add getter and setter to mock
        self.dbus_mock.AddMethods(
            'org.freedesktop.DBus.Properties',
            [
                ('self.Get',
                 's',
                 'v',
                 'ret = self.accts_snd_props[args[0]]'),
                ('self.Set',
                 'sv',
                 '',
                 'self.accts_snd_props[args[0]] = args[1]')
            ])

        # add user object to mock
        self.dbus_mock.AddObject(
            user_obj, ACCOUNTS_SOUND_IFACE, self.accts_snd_props,
            [
                (
                    'GetIncomingCallVibrate', '', 'v',
                    'ret = self.Get("%s", "IncomingCallVibrate")' %
                    ACCOUNTS_SOUND_IFACE),
                (
                    'GetIncomingMessageVibrate', '', 'v',
                    'ret = self.Get("%s", "IncomingMessageVibrate")' %
                    ACCOUNTS_SOUND_IFACE),
                (
                    'GetIncomingCallVibrateSilentMode', '', 'v',
                    'ret = self.Get("%s", "IncomingCallVibrateSilentMode")' %
                    ACCOUNTS_SOUND_IFACE),
                (
                    'GetIncomingMessageVibrateSilentMode', '', 'v',
                    'ret = self.Get("%s", \
                                    "IncomingMessageVibrateSilentMode")' %
                    ACCOUNTS_SOUND_IFACE),
                (
                    'GetDialpadSoundsEnabled', '', 'v',
                    'ret = self.Get("%s", \
                                    "DialpadSoundsEnabled")' %
                    ACCOUNTS_SOUND_IFACE)
            ])

        self.obj_test = self.dbus_con.get_object(ACCOUNTS_IFACE, user_obj,
                                                 ACCOUNTS_IFACE)

        self.dbus_mock_isound.AddMethods(
            GTK_ACTIONS_IFACE,
            [
                (
                    'Activate', 'sava{sv}', '',
                    ''
                ),
                (
                    'Describe', 's', '(bsav)',
                    'if args[0] == "silent-mode":'
                    '    ret = [True, "", [False]]'
                    'if args[0] == "volume":'
                    '    ret = [True, "i", [0.4]]'
                ),
                (
                    'DescribeAll', 's', 'a{s(bsav)}',
                    'ret = {'
                    '"silent-mode": [True, "", [False]],'
                    '"volume": [True, "i", [0.4]]}'
                ),
                (
                    'List', '', 'as',
                    'ret = ["silent-mode", "volume"]'
                ),
                (
                    'SetState', 'sva{sv}', '',
                    ''
                )
            ])

        super(SoundBaseTestCase, self).setUp(panel)

        """ Go to Sound page """
        if panel == 'sound':
            self.assertThat(self.system_settings.main_view.sound_page.active,
                            Eventually(Equals(True)))

    def tearDown(self):
        self.mock_server.terminate()
        self.mock_server.wait()
        self.mock_isound.terminate()
        self.mock_isound.wait()
        super(SoundBaseTestCase, self).tearDown()


class ResetBaseTestCase(UbuntuSystemSettingsTestCase,
                        dbusmock.DBusTestCase):
    """ Base class for reset settings tests"""

    def mock_for_factory_reset(self):
        self.mock_server = self.spawn_server(SYSTEM_IFACE, SYSTEM_SERVICE_OBJ,
                                             SYSTEM_IFACE, system_bus=True,
                                             stdout=subprocess.PIPE)
        # spawn_server does not wait properly
        # Reported as bug here: http://pad.lv/1350833
        sleep(2)
        self.sys_mock = dbus.Interface(self.dbus_con.get_object(
            SYSTEM_IFACE, SYSTEM_SERVICE_OBJ), dbusmock.MOCK_IFACE)

        self.sys_mock.AddMethod(SYSTEM_IFACE, 'FactoryReset', '', '', '')

    @classmethod
    def setUpClass(klass):
        klass.start_system_bus()
        klass.dbus_con = klass.get_dbus(True)

    def setUp(self):
        self.mock_for_factory_reset()
        super(ResetBaseTestCase, self).setUp()
        self.reset_page = self.system_settings.main_view.go_to_reset_phone()

    def tearDown(self):
        self.mock_server.terminate()
        self.mock_server.wait()
        super(ResetBaseTestCase, self).tearDown()


class SecurityBaseTestCase(UbuntuSystemSettingsOfonoTestCase):
    """ Base class for security and privacy settings tests"""

    def setUp(self):
        super(SecurityBaseTestCase, self).setUp('security-privacy')
        """ Go to Security & Privacy page """
        self.security_page = self.system_settings.main_view.select_single(
            objectName='securityPrivacyPage'
        )
        self.assertThat(self.security_page.active,
                        Eventually(Equals(True)))

    def tearDown(self):
        super(SecurityBaseTestCase, self).tearDown()


class PhoneSoundBaseTestCase(SoundBaseTestCase):
    def setUp(self):
        """ Go to Phone page """
        super(PhoneSoundBaseTestCase, self).setUp('phone')
        self.phone_page = self.system_settings.main_view.select_single(
            objectName='phonePage'
        )

    def tearDown(self):
        super(PhoneSoundBaseTestCase, self).tearDown()


class LanguageBaseTestCase(UbuntuSystemSettingsTestCase,
                           dbusmock.DBusTestCase):
    """ Base class for language settings tests"""

    def mock_loginmanager(self):
        self.mock_server = self.spawn_server(LM_SERVICE, LM_PATH,
                                             LM_IFACE, system_bus=True,
                                             stdout=subprocess.PIPE)
        # spawn_server does not wait properly
        # Reported as bug here: http://pad.lv/1350833
        sleep(2)
        self.session_mock = dbus.Interface(self.dbus_con.get_object(
            LM_SERVICE, LM_PATH), dbusmock.MOCK_IFACE)

        self.session_mock.AddMethod(LM_IFACE, 'Reboot', 'b', '', '')

    @classmethod
    def setUpClass(klass):
        klass.start_system_bus()
        klass.dbus_con = klass.get_dbus(True)

    def setUp(self):
        self.mock_loginmanager()

        super(LanguageBaseTestCase, self).setUp()
        self.language_page = self.system_settings.\
            main_view.go_to_language_page()

    def tearDown(self):
        self.mock_server.terminate()
        self.mock_server.wait()
        super(LanguageBaseTestCase, self).tearDown()


class WifiBaseTestCase(UbuntuSystemSettingsTestCase,
                       dbusmock.DBusTestCase):
    """ Base class for wifi settings tests"""

    @classmethod
    def setUpClass(cls):
        cls.start_system_bus()
        cls.dbus_con = cls.get_dbus(True)
        # Add a mock NetworkManager environment so we get consistent results
        (cls.p_mock, cls.obj_nm) = cls.spawn_server_template(
            'networkmanager', stdout=subprocess.PIPE)
        cls.dbusmock = dbus.Interface(cls.obj_nm, dbusmock.MOCK_IFACE)

    def setUp(self, panel=None):
        self.obj_nm.Reset()
        device_path = self.obj_nm.AddWiFiDevice('test0', 'Barbaz', 1)
        self.device_mock = dbus.Interface(self.dbus_con.get_object(
            'org.freedesktop.NetworkManager', device_path),
            dbusmock.MOCK_IFACE)

        """A device should not just implement Device.Wireless/Device.Wired
        interfaces, but also the Device interface. Since we want to test
        the Disconnect method, we add it."""

        try:
            self.device_mock.AddMethod(DEVICE_IFACE, 'Disconnect', '', '', '')
        except:
            # it was already added
            pass

        super(WifiBaseTestCase, self).setUp()
        self.wifi_page = self.system_settings.\
            main_view.go_to_wifi_page()

    def add_previous_networks(self, networks):
        dev_path = str(self.obj_nm.GetDevices()[0])
        for network in networks:
            self.obj_nm.AddWiFiConnection(
                dev_path, network['connection_name'],
                network['ssid'], network.get('keymng', ''))
