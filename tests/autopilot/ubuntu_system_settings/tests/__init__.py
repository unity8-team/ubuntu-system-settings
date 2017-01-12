# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2013, 2014, 2015 Canonical
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation; version 3.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.

""" Tests for Ubuntu System Settings """

from __future__ import absolute_import

import dbus
import dbusmock
import os
import random
import subprocess
import ubuntuuitoolkit

from datetime import datetime
from time import sleep

from autopilot import platform
from autopilot.matchers import Eventually
from dbusmock.templates.networkmanager import (InfrastructureMode,
                                               NM80211ApSecurityFlags)
from fixtures import EnvironmentVariable
from gi.repository import UPowerGlib
from testtools.matchers import Equals, NotEquals, GreaterThan
from ubuntu_system_settings.utils.mock_update_click_server import (
    Manager
)
from ubuntu_system_settings.tests.connectivity import (
    PRIV_OBJ as CTV_PRIV_OBJ, NETS_OBJ as CTV_NETS_OBJ,
    MAIN_IFACE as CTV_IFACE
)
from ubuntuuitoolkit._custom_proxy_objects._common import (
    is_process_running, _start_process, _stop_process)


ACCOUNTS_IFACE = 'org.freedesktop.Accounts'
ACCOUNTS_USER_IFACE = 'org.freedesktop.Accounts.User'
ACCOUNTS_OBJ = '/org/freedesktop/Accounts'
ACCOUNTS_SERVICE = 'com.canonical.unity.AccountsService'
ACCOUNTS_SOUND_IFACE = 'com.ubuntu.touch.AccountsService.Sound'
ACCOUNTS_PHONE_IFACE = 'com.ubuntu.touch.AccountsService.Phone'
INDICATOR_NETWORK = 'indicator-network'
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
NM_AC_CON_IFACE = 'org.freedesktop.NetworkManager.Connection.Active'
CON_SERVICE = 'com.ubuntu.connectivity1'
CON_PATH = '/com/ubuntu/connectivity1/Private'
CON_IFACE = 'com.ubuntu.connectivity1.Private'
UPOWER_VERSION = str(UPowerGlib.MAJOR_VERSION)
UPOWER_VERSION += '.' + str(UPowerGlib.MINOR_VERSION)


class UbuntuSystemSettingsTestCase(
        ubuntuuitoolkit.base.UbuntuUIToolkitAppTestCase):

    """Base class for Ubuntu System Settings."""

    BINARY = 'system-settings'
    DESKTOP_FILE = '/usr/share/applications/ubuntu-system-settings.desktop'

    def setUp(self, panel=None):
        super(UbuntuSystemSettingsTestCase, self).setUp()
        self.system_settings = self.launch(panel)
        self.main_view = self.system_settings.main_view
        self.assertThat(
            self.main_view.visible,
            Eventually(Equals(True)))

    def set_orientation(self, gsettings, value):
        gsettings.set_value('rotation-lock', value)
        self.assertThat(
            lambda: gsettings.get_value('rotation-lock').get_boolean(),
            Eventually(Equals(value.get_boolean())))

    def launch(self, panel=None):
        """Launch system settings application

        :param testobj: An AutopilotTestCase object, needed to call
        testobj.launch_test_application()

        :param panel: Launch to a specific panel. Default None.

        :returns: A proxy object that represents the application. Introspection
        data is retrievable via this object.
        """
        params = [self.BINARY]
        if platform.model() != 'Desktop':
            params.append('--desktop_file_hint={}'.format(self.DESKTOP_FILE))

        # Launch to a specific panel
        if panel is not None:
            params.append(panel)

        return self.launch_test_application(
            *params,
            app_type='qt',
            emulator_base=ubuntuuitoolkit.UbuntuUIToolkitCustomProxyObjectBase,
            capture_output=True)

    @classmethod
    def tearDownClass(cls):
        if dbusmock.DBusTestCase.system_bus_pid is not None:
            cls.stop_dbus(dbusmock.DBusTestCase.system_bus_pid)
            del os.environ['DBUS_SYSTEM_BUS_ADDRESS']
            dbusmock.DBusTestCase.system_bus_pid = None
        if dbusmock.DBusTestCase.session_bus_pid is not None:
            cls.stop_dbus(dbusmock.DBusTestCase.session_bus_pid)
            del os.environ['DBUS_SESSION_BUS_ADDRESS']
            dbusmock.DBusTestCase.session_bus_pid = None
        super(UbuntuSystemSettingsTestCase, cls).tearDownClass()


class UbuntuSystemSettingsUpowerTestCase(UbuntuSystemSettingsTestCase,
                                         dbusmock.DBusTestCase):
    """Base class for battery tests that mocks Upower"""

    @classmethod
    def setUpClass(cls):
        cls.start_system_bus()
        cls.dbus_con = cls.get_dbus(True)

    def setUp(self, panel=None):
        # Add a mock Upower environment so we get consistent results
        (self.p_mock, self.obj_upower) = self.spawn_server_template(
            'upower',
            {'OnBattery': True, 'DaemonVersion': UPOWER_VERSION},
            stdout=subprocess.PIPE)
        self.dbusmock = dbus.Interface(self.obj_upower, dbusmock.MOCK_IFACE)
        self.obj_upower.Reset()
        super(UbuntuSystemSettingsUpowerTestCase, self).setUp()

    def add_mock_battery(self):
        """ Make sure we have a battery """
        self.dbusmock.AddDischargingBattery(
            'mock_BATTERY', 'Battery', 50.0, 10
        )

    def tearDown(self):
        self.p_mock.terminate()
        self.p_mock.wait()
        super(UbuntuSystemSettingsUpowerTestCase, self).tearDown()


class UbuntuSystemSettingsBatteryTestCase(UbuntuSystemSettingsUpowerTestCase):
    """ Base class for tests which rely on the presence of a battery """

    def setUp(self):
        super(UbuntuSystemSettingsBatteryTestCase, self).setUp()
        self.add_mock_battery()


class UbuntuSystemSettingsHotspotTestCase(UbuntuSystemSettingsTestCase,
                                          dbusmock.DBusTestCase):
    """Base class for tests that tests the hotspot functionality."""
    connectivity_parameters = {}
    indicatornetwork_parameters = {}
    systemimage_parameters = {'device': 'ideal'}

    @classmethod
    def setUpClass(cls):
        cls.session_con = cls.get_dbus(False)

        cls.start_system_bus()

        si_tmpl = os.path.join(os.path.dirname(__file__), 'systemimage.py')
        (cls.si_mock, cls.si_obj) = cls.spawn_server_template(
            si_tmpl, parameters=cls.systemimage_parameters,
            stdout=subprocess.PIPE)

        super(UbuntuSystemSettingsHotspotTestCase, cls).setUpClass()

    def setUp(self):
        if is_process_running(INDICATOR_NETWORK):
            _stop_process(INDICATOR_NETWORK)
            self.addCleanup(_start_process, INDICATOR_NETWORK)

        ctv_tmpl = os.path.join(os.path.dirname(__file__), 'connectivity.py')
        (self.ctv_mock, self.obj_ctv) = self.spawn_server_template(
            ctv_tmpl, parameters=self.connectivity_parameters,
            stdout=subprocess.PIPE)

        self.ctv_private = dbus.Interface(
            self.session_con.get_object(CTV_IFACE, CTV_PRIV_OBJ),
            'org.freedesktop.DBus.Properties')

        self.ctv_nets = dbus.Interface(
            self.session_con.get_object(CTV_IFACE, CTV_NETS_OBJ),
            'org.freedesktop.DBus.Properties')

        inetwork = os.path.join(
            os.path.dirname(__file__), 'indicatornetwork.py'
        )
        (self.inetwork_mock, self.obj_inetwork) = self.spawn_server_template(
            inetwork, parameters=self.indicatornetwork_parameters,
            stdout=subprocess.PIPE)

        # Required since this test needs to dismiss the OSK.
        self.useFixture(EnvironmentVariable("UITK_USE_MALIIT", "1"))

        super(UbuntuSystemSettingsHotspotTestCase, self).setUp()

    def tearDown(self):
        self.ctv_mock.terminate()
        self.ctv_mock.wait()
        self.inetwork_mock.terminate()
        self.inetwork_mock.wait()
        super(UbuntuSystemSettingsHotspotTestCase, self).tearDown()

    @classmethod
    def tearDownClass(cls):
        cls.si_mock.terminate()
        cls.si_mock.wait()
        if dbusmock.DBusTestCase.system_bus_pid is not None:
            cls.stop_dbus(dbusmock.DBusTestCase.system_bus_pid)
            del os.environ['DBUS_SYSTEM_BUS_ADDRESS']
            dbusmock.DBusTestCase.system_bus_pid = None
        if dbusmock.DBusTestCase.session_bus_pid is not None:
            cls.stop_dbus(dbusmock.DBusTestCase.session_bus_pid)
            del os.environ['DBUS_SESSION_BUS_ADDRESS']
            dbusmock.DBusTestCase.session_bus_pid = None
        super(UbuntuSystemSettingsHotspotTestCase, cls).tearDownClass()


class UbuntuSystemSettingsOfonoTestCase(UbuntuSystemSettingsTestCase,
                                        dbusmock.DBusTestCase):
    """Class for cellular tests which sets up an Ofono mock """

    use_sims = 1

    @property
    def choose_carrier_page(self):
        """Return carrier selection page"""
        return self.main_view.select_single(
            objectName='chooseCarrierPage'
        )

    @property
    def choose_carriers_page(self):
        """Return carriers selection page"""
        return self.main_view.select_single(
            objectName='chooseCarriersPage'
        )

    # TODO: remove this when it has been fixed in dbusmock
    def get_all_operators(self, name):
        return 'ret = [(m, objects[m].GetAll("org.ofono.NetworkOperator")) ' \
               'for m in objects if "%s/operator/" in m]' % name

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
            RDO_IFACE, 'AvailableTechnologies', technologies)
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
        modem.AddProperties(CALL_FWD_IFACE,
                            {
                                'VoiceUnconditional': '',
                                'VoiceBusy': '',
                                'VoiceNoReply': '',
                                'VoiceNotReachable': '',
                            })
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
        self.mock_call_forwarding(self.modem_0)
        self.mock_call_settings(self.modem_0)

        self.modem_0.AddMethods('org.ofono.NetworkRegistration', [
            ('GetProperties', '', 'a{sv}',
                'ret = self.GetAll("org.ofono.NetworkRegistration")'),
            ('Register', '', '', ''),
            ('GetOperators', '', 'a(oa{sv})', self.get_all_operators('ril_0')),
            ('Scan', '', 'a(oa{sv})', self.get_all_operators('ril_0')),
        ])
        self.modem_0.connMan = dbus.Interface(self.dbus_con.get_object(
                                              'org.ofono', '/ril_0'),
                                              'org.ofono.ConnectionManager')

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
        self.mock_call_forwarding(self.modem_1)
        self.mock_call_settings(self.modem_1)

        self.modem_1.Set(
            SIM_IFACE,
            'SubscriberNumbers', ['08123', '938762783']
        )

        self.modem_1.connMan = dbus.Interface(self.dbus_con.get_object(
                                              'org.ofono', '/' + second_modem),
                                              'org.ofono.ConnectionManager')

    @classmethod
    def setUpClass(cls):
        cls.start_system_bus()
        cls.dbus_con = cls.get_dbus(True)
        super(UbuntuSystemSettingsOfonoTestCase, cls).setUpClass()

    def tearDown(self):
        self.p_mock.terminate()
        self.p_mock.wait()
        super(UbuntuSystemSettingsOfonoTestCase, self).tearDown()

    def setUp(self, panel=None):
        template = os.path.join(os.path.dirname(__file__), 'ofono.py')
        # Add a mock Ofono environment so we get consistent results
        (self.p_mock, self.obj_ofono) = self.spawn_server_template(
            template, stdout=subprocess.PIPE)
        self.dbusmock = dbus.Interface(self.obj_ofono, dbusmock.MOCK_IFACE)
        self.obj_ofono.Reset()

        self.add_sim1()
        if self.use_sims == 2:
            self.add_sim2()

        super(UbuntuSystemSettingsOfonoTestCase, self).setUp()

    def get_default_sim_for_calls_selector(self, text):
        return self.cellular_page.select_single(
            objectName="defaultForCalls" + text
        )

    def get_default_sim_for_messages_selector(self, text):
        return self.cellular_page.select_single(
            objectName="defaultForMessages" + text
        )


class CellularBaseTestCase(UbuntuSystemSettingsOfonoTestCase):

    connectivity_parameters = {
        'Status': 'online'
    }

    def setUp(self):
        """ Go to Cellular page """

        self.session_con = self.get_dbus(False)

        if is_process_running(INDICATOR_NETWORK):
            _stop_process(INDICATOR_NETWORK)
            self.addCleanup(_start_process, INDICATOR_NETWORK)

        ctv_tmpl = os.path.join(os.path.dirname(__file__), 'connectivity.py')
        (self.ctv_mock, self.obj_ctv) = self.spawn_server_template(
            ctv_tmpl, parameters=self.connectivity_parameters,
            stdout=subprocess.PIPE)

        sleep(1)

        self.ctv_private = dbus.Interface(
            self.session_con.get_object(CTV_IFACE, CTV_PRIV_OBJ),
            'org.freedesktop.DBus.Properties')

        sim = self.obj_ctv.AddSim("1234567890")
        self.ctv_private.Set(CON_IFACE,
                             'Sims',
                             dbus.Array([sim],
                                        signature='o'))
        modem = self.obj_ctv.AddModem("0987654321", 1, sim)
        self.ctv_private.Set(CON_IFACE,
                             'Modems',
                             dbus.Array([modem],
                                        signature='o'))

        self.ctv_modem0 = dbus.Interface(
            self.session_con.get_object(CTV_IFACE, modem),
            'org.freedesktop.DBus.Properties')
        self.ctv_sim0 = dbus.Interface(
            self.session_con.get_object(CTV_IFACE, sim),
            'org.freedesktop.DBus.Properties')

        if self.use_sims == 2:
            sim2 = self.obj_ctv.AddSim("2345678901")
            self.ctv_private.Set(CON_IFACE,
                                 'Sims',
                                 dbus.Array([sim, sim2],
                                            signature='o'))
            modem2 = self.obj_ctv.AddModem("1098765432", 2, sim2)
            self.ctv_private.Set(CON_IFACE,
                                 'Modems',
                                 dbus.Array([modem, modem2],
                                            signature='o'))

            self.ctv_modem1 = dbus.Interface(
                self.session_con.get_object(CTV_IFACE, modem2),
                'org.freedesktop.DBus.Properties')
            self.ctv_sim1 = dbus.Interface(
                self.session_con.get_object(CTV_IFACE, sim2),
                'org.freedesktop.DBus.Properties')

        user_obj = '/user/foo'

        self.accts_phone_props = {
            'DefaultSimForCalls': dbus.String("/ril_0", variant_level=1),
            'DefaultSimForMessages': dbus.String("/ril_1", variant_level=1),
            'SimNames': dbus.Dictionary({}, signature='ss', variant_level=1)}

        # start dbus system bus
        self.mock_server = self.spawn_server(ACCOUNTS_IFACE, ACCOUNTS_OBJ,
                                             ACCOUNTS_IFACE, system_bus=True,
                                             stdout=subprocess.PIPE)
        # give it time to ensure the mock is up
        sleep(1)

        self.wait_for_bus_object(ACCOUNTS_IFACE,
                                 ACCOUNTS_OBJ,
                                 system_bus=True)

        self.dbus_mock = dbus.Interface(self.dbus_con.get_object(
                                        ACCOUNTS_IFACE,
                                        ACCOUNTS_OBJ,
                                        ACCOUNTS_IFACE),
                                        dbusmock.MOCK_IFACE)

        # let accountservice find a user object path
        self.dbus_mock.AddMethod(ACCOUNTS_IFACE, 'FindUserById', 'x', 'o',
                                 'ret = "%s"' % user_obj)

        self.dbus_mock.AddProperties(ACCOUNTS_PHONE_IFACE,
                                     self.accts_phone_props)

        # add getter and setter to mock
        self.dbus_mock.AddMethods(
            'org.freedesktop.DBus.Properties',
            [
                ('self.Get',
                 's',
                 'v',
                 'ret = self.accts_phone_props[args[0]]'),
                ('self.Set',
                 'sv',
                 '',
                 'self.accts_phone_props[args[0]] = args[1]')
            ])

        # add user object to mock
        self.dbus_mock.AddObject(
            user_obj, ACCOUNTS_PHONE_IFACE, self.accts_phone_props,
            [
                (
                    'GetDefaultSimForCalls', '', 'v',
                    'ret = self.Get("%s", "DefaultSimForCalls")' %
                    ACCOUNTS_PHONE_IFACE),
                (
                    'GetDefaultSimForMessages', '', 'v',
                    'ret = self.Get("%s", "DefaultSimForMessages")' %
                    ACCOUNTS_PHONE_IFACE),
                (
                    'GetSimNames', '', 'v',
                    'ret = self.Get("%s", "SimNames")' %
                    ACCOUNTS_PHONE_IFACE)
            ])

        self.obj_phone = self.dbus_con.get_object(ACCOUNTS_IFACE, user_obj,
                                                  ACCOUNTS_IFACE)

        super(CellularBaseTestCase, self).setUp()
        self.cellular_page = self.main_view.go_to_cellular_page()

    def add_connection_context(self, modem, **kwargs):
        iface = 'org.ofono.ConnectionContext'
        path = modem.connMan.AddContext(kwargs.get('Type', 'internet'))
        context = dbus.Interface(self.dbus_con.get_object(
                                 'org.ofono', path),
                                 iface)

        for key, value in kwargs.items():
            context.SetProperty(key, value)

    def tearDown(self):
        self.ctv_mock.terminate()
        self.ctv_mock.wait()
        self.mock_server.terminate()
        self.mock_server.wait()
        super(CellularBaseTestCase, self).tearDown()


class HotspotBaseTestCase(UbuntuSystemSettingsHotspotTestCase):

    def setUp(self):
        super(HotspotBaseTestCase, self).setUp()
        self.hotspot_page = self.main_view.go_to_hotspot_page()


class BluetoothBaseTestCase(UbuntuSystemSettingsTestCase):

    def setUp(self):
        """ Go to Bluetooth page """
        super(BluetoothBaseTestCase, self).setUp()
        self.bluetooth_page = self.main_view.go_to_bluetooth_page()


class PhoneOfonoBaseTestCase(UbuntuSystemSettingsOfonoTestCase):
    def setUp(self):
        """ Go to Phone page """
        self.useFixture(EnvironmentVariable("USS_SHOW_ALL_UI", "1"))
        super(PhoneOfonoBaseTestCase, self).setUp()
        self.phone_page = self.main_view.go_to_phone_page()


class AboutBaseTestCase(UbuntuSystemSettingsTestCase):
    def setUp(self):
        """Go to About page."""
        super(AboutBaseTestCase, self).setUp()
        self.about_page = self.main_view.go_to_about_page()


class AboutOfonoBaseTestCase(UbuntuSystemSettingsOfonoTestCase):
    def setUp(self):
        """Go to About page."""
        super(AboutOfonoBaseTestCase, self).setUp()
        self.about_page = self.main_view.go_to_about_page()


class AboutSystemImageBaseTestCase(AboutBaseTestCase,
                                   dbusmock.DBusTestCase):
    systemimage_parameters = {
        'last_update_date': datetime.now().replace(microsecond=0).isoformat(),
        'channel': 'daily',
        'build_number': 42,
        'device': ''
    }

    @classmethod
    def setUpClass(cls):
        cls.start_system_bus()
        cls.dbus_con = cls.get_dbus(True)
        si_tmpl = os.path.join(os.path.dirname(__file__), 'systemimage.py')
        (cls.si_mock, cls.si_obj) = cls.spawn_server_template(
            si_tmpl, parameters=cls.systemimage_parameters,
            stdout=subprocess.PIPE)

    @classmethod
    def tearDownClass(cls):
        cls.si_mock.terminate()
        cls.si_mock.wait()
        super(AboutSystemImageBaseTestCase, cls).tearDownClass()


class StorageBaseTestCase(AboutBaseTestCase):

    """Base class for Storage page tests."""

    def setUp(self):
        """Go to Storage Page."""
        super(StorageBaseTestCase, self).setUp()
        self.main_view.click_item('storageItem')
        self.storage_page = self.main_view.select_single(
            objectName='storagePage'
        )
        self.assertThat(self.storage_page.active, Eventually(Equals(True)))

    def assert_space_item(self, object_name, text):
        """ Checks whether an space item exists and returns a value """
        item = self.main_view.storage_page.wait_select_single(
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


class LicenseBaseTestCase(AboutBaseTestCase):

    """Base class for Licenses page tests."""

    def setUp(self):
        """Go to License Page."""
        super(LicenseBaseTestCase, self).setUp()
        self.licenses_page = self.about_page.go_to_software_licenses()


class SystemUpdatesBaseTestCase(UbuntuSystemSettingsTestCase,
                                dbusmock.DBusTestCase):
    """Base class for SystemUpdates page tests."""

    connectivity_parameters = {
        'Status': 'online'
    }

    click_server_parameters = {
        'start': False
    }

    systemimage_parameters = {}

    @classmethod
    def setUpClass(cls):
        cls.session_con = cls.get_dbus(False)

        cls.start_system_bus()

        si_tmpl = os.path.join(os.path.dirname(__file__), 'systemimage.py')
        (cls.si_mock, cls.si_obj) = cls.spawn_server_template(
            si_tmpl, parameters=cls.systemimage_parameters,
            stdout=subprocess.PIPE)

        super(SystemUpdatesBaseTestCase, cls).setUpClass()

    def setUp(self):
        """Go to SystemUpdates Page."""
        self.clicksrv_manager = None
        if is_process_running(INDICATOR_NETWORK):
            _stop_process(INDICATOR_NETWORK)
            self.addCleanup(_start_process, INDICATOR_NETWORK)

        ctv_tmpl = os.path.join(os.path.dirname(__file__), 'connectivity.py')
        (self.ctv_mock, self.obj_ctv) = self.spawn_server_template(
            ctv_tmpl, parameters=self.connectivity_parameters,
            stdout=subprocess.PIPE)

        self.ctv_nets = dbus.Interface(
            self.session_con.get_object(CTV_IFACE, CTV_NETS_OBJ),
            'org.freedesktop.DBus.Properties')

        if self.click_server_parameters['start']:
            self.clicksrv_manager = Manager(
                responses=self.click_server_parameters.get('responses', None)
            )
            self.clicksrv_manager.start()

        super(SystemUpdatesBaseTestCase, self).setUp()
        self.main_view.click_item('entryComponent-system-update')

    def tearDown(self):
        self.ctv_mock.terminate()
        self.ctv_mock.wait()
        if self.clicksrv_manager and self.clicksrv_manager.is_running():
            self.clicksrv_manager.stop()
        super(SystemUpdatesBaseTestCase, self).tearDown()

    @classmethod
    def tearDownClass(cls):
        cls.si_mock.terminate()
        cls.si_mock.wait()
        if dbusmock.DBusTestCase.system_bus_pid is not None:
            cls.stop_dbus(dbusmock.DBusTestCase.system_bus_pid)
            del os.environ['DBUS_SYSTEM_BUS_ADDRESS']
            dbusmock.DBusTestCase.system_bus_pid = None
        if dbusmock.DBusTestCase.session_bus_pid is not None:
            cls.stop_dbus(dbusmock.DBusTestCase.session_bus_pid)
            del os.environ['DBUS_SESSION_BUS_ADDRESS']
            dbusmock.DBusTestCase.session_bus_pid = None
        super(SystemUpdatesBaseTestCase, cls).tearDownClass()


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
        self.assertThat(self.main_view.background_page.active,
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
        klass.dbus_con = klass.get_dbus(True)
        klass.dbus_con_session = klass.get_dbus(False)

    def setUp(self, panel='sound'):
        # TODO only do this if the sound indicator is running.
        # --elopio - 2015-01-08
        self.stop_sound_indicator()
        self.addCleanup(self.start_sound_indicator)

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

        self.wait_for_bus_object(ACCOUNTS_IFACE,
                                 ACCOUNTS_OBJ,
                                 system_bus=True)

        self.dbus_mock = dbus.Interface(self.dbus_con.get_object(
                                        ACCOUNTS_IFACE,
                                        ACCOUNTS_OBJ,
                                        ACCOUNTS_IFACE),
                                        dbusmock.MOCK_IFACE)

        self.wait_for_bus_object(ISOUND_SERVICE,
                                 ISOUND_ACTION_PATH)

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

        self.obj_snd = self.dbus_con.get_object(ACCOUNTS_IFACE, user_obj,
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

        if panel == 'sound':
            self.sound_page = self.main_view.select_single(
                objectName='soundPage'
            )
            self.assertThat(self.sound_page.active,
                            Eventually(Equals(True)))

    def tearDown(self):
        self.mock_server.terminate()
        self.mock_server.wait()
        self.mock_isound.terminate()
        self.mock_isound.wait()
        super(SoundBaseTestCase, self).tearDown()

    def start_sound_indicator(self):
        subprocess.call(['initctl', 'start', 'indicator-sound'])

    def stop_sound_indicator(self):
        subprocess.call(['initctl', 'stop', 'indicator-sound'])


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
        self.reset_page = self.main_view.go_to_reset_phone()

    def tearDown(self):
        self.mock_server.terminate()
        self.mock_server.wait()
        super(ResetBaseTestCase, self).tearDown()


class SecurityBaseTestCase(UbuntuSystemSettingsOfonoTestCase):
    """ Base class for security and privacy settings tests"""

    def setUp(self):
        super(SecurityBaseTestCase, self).setUp()
        self.security_page = self.main_view.go_to_security_page()

    def tearDown(self):
        super(SecurityBaseTestCase, self).tearDown()


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
        self.language_page = self.main_view.go_to_language_page()

    def tearDown(self):
        self.mock_server.terminate()
        self.mock_server.wait()
        super(LanguageBaseTestCase, self).tearDown()


class UbuntuSystemSettingsVpnTestCase(UbuntuSystemSettingsTestCase,
                                      dbusmock.DBusTestCase):
    """Base class for tests that tests the vpn functionality."""
    connectivity_parameters = {}
    indicatornetwork_parameters = {}

    @classmethod
    def setUpClass(cls):
        cls.session_con = cls.get_dbus(False)

        cls.start_system_bus()

        super(UbuntuSystemSettingsVpnTestCase, cls).setUpClass()

    def setUp(self):
        if is_process_running(INDICATOR_NETWORK):
            _stop_process(INDICATOR_NETWORK)
            self.addCleanup(_start_process, INDICATOR_NETWORK)

        ctv_tmpl = os.path.join(os.path.dirname(__file__), 'connectivity.py')
        (self.ctv_mock, self.obj_ctv) = self.spawn_server_template(
            ctv_tmpl, parameters=self.connectivity_parameters,
            stdout=subprocess.PIPE)

        self.ctv_private = dbus.Interface(
            self.session_con.get_object(CTV_IFACE, CTV_PRIV_OBJ),
            'org.freedesktop.DBus.Properties')

        super(UbuntuSystemSettingsVpnTestCase, self).setUp()

    def tearDown(self):
        self.ctv_mock.terminate()
        self.ctv_mock.wait()
        super(UbuntuSystemSettingsVpnTestCase, self).tearDown()

    @classmethod
    def tearDownClass(cls):
        if dbusmock.DBusTestCase.system_bus_pid is not None:
            cls.stop_dbus(dbusmock.DBusTestCase.system_bus_pid)
            del os.environ['DBUS_SYSTEM_BUS_ADDRESS']
            dbusmock.DBusTestCase.system_bus_pid = None
        if dbusmock.DBusTestCase.session_bus_pid is not None:
            cls.stop_dbus(dbusmock.DBusTestCase.session_bus_pid)
            del os.environ['DBUS_SESSION_BUS_ADDRESS']
            dbusmock.DBusTestCase.session_bus_pid = None
        super(UbuntuSystemSettingsVpnTestCase, cls).tearDownClass()


class VpnBaseTestCase(UbuntuSystemSettingsVpnTestCase):

    def setUp(self):
        super(VpnBaseTestCase, self).setUp()
        # Required since this test needs to dismiss the OSK.
        self.useFixture(EnvironmentVariable("UITK_USE_MALIIT", "1"))
        self.vpn_page = self.main_view.go_to_vpn_page()

    def get_vpn_connection_object(self, path):
        return dbus.Interface(
            self.session_con.get_object(CTV_IFACE, path),
            'org.freedesktop.DBus.Properties'
        )


class WifiBaseTestCase(UbuntuSystemSettingsTestCase,
                       dbusmock.DBusTestCase):
    """ Base class for wifi settings tests"""

    indicatornetwork_parameters = {}

    @classmethod
    def setUpClass(cls):
        cls.start_system_bus()
        cls.dbus_con = cls.get_dbus(True)
        cls.session_con = cls.get_dbus(False)

        template = os.path.join(os.path.dirname(__file__), 'networkmanager.py')
        (cls.p_mock, cls.obj_nm) = cls.spawn_server_template(
            template, stdout=subprocess.PIPE)
        super(WifiBaseTestCase, cls).setUpClass()

    def setUp(self, panel=None):
        if is_process_running(INDICATOR_NETWORK):
            _stop_process(INDICATOR_NETWORK)
            self.addCleanup(_start_process, INDICATOR_NETWORK)

        inetwork = os.path.join(
            os.path.dirname(__file__), 'indicatornetwork.py'
        )
        (self.inetwork_mock, self.obj_inetwork) = self.spawn_server_template(
            inetwork, parameters=self.indicatornetwork_parameters,
            stdout=subprocess.PIPE)

        self.obj_nm.Reset()

        # Add a mock NetworkManager environment so we get consistent results
        self.device_path = self.obj_nm.AddWiFiDevice('test0', 'Barbaz', 1)
        self.device_mock = dbus.Interface(self.dbus_con.get_object(
            NM_SERVICE, self.device_path),
            dbusmock.MOCK_IFACE)

        self.ap_mock = self.create_access_point(
            'test_ap', 'test_ap',
            security=NM80211ApSecurityFlags.NM_802_11_AP_SEC_KEY_MGMT_PSK
        )

        # Required since this test needs to dismiss the OSK.
        self.useFixture(EnvironmentVariable("UITK_USE_MALIIT", "1"))

        super(WifiBaseTestCase, self).setUp(panel)
        if panel:
            self.wifi_page = self.main_view.wait_select_single(
                objectName='wifiPage'
            )
        else:
            self.wifi_page = self.main_view.go_to_wifi_page()

        self.wifi_page._scroll_to_and_click = \
            self.main_view.scroll_to_and_click

    def tearDown(self):
        self.inetwork_mock.terminate()
        self.inetwork_mock.wait()
        super(WifiBaseTestCase, self).tearDown()

    def create_access_point(self, name, ssid, security=None):
        """Creates access point.

        :param name: Name of access point
        :param ssid: SSID of access point
        :param security: Either None, or a NM80211ApSecurityFlags

        :returns: Access point

        """
        if security is None:
            security = NM80211ApSecurityFlags.NM_802_11_AP_SEC_NONE

        return self.obj_nm.AddAccessPoint(
            self.device_path, name, ssid, self.random_mac_address(),
            InfrastructureMode.NM_802_11_MODE_INFRA, 2425, 5400, 82, security)

    def random_mac_address(self):
        """Returns a random Mac Address"""
        mac = [0x00, 0x16, 0x3e, random.randint(0x00, 0x7f),
               random.randint(0x00, 0xff), random.randint(0x00, 0xff)]
        return ':'.join(map(lambda x: "%02x" % x, mac))


class WifiWithSSIDBaseTestCase(WifiBaseTestCase):
    """ Class for Wi-Fi settings tests launches with an SSID."""

    ssid = None

    def setUp(self, panel=None):
        super(WifiWithSSIDBaseTestCase, self).setUp(
            panel='settings:///wifi/?ssid=%s' % self.ssid
        )
