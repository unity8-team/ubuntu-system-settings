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
from time import sleep

import ubuntuuitoolkit
from autopilot.matchers import Eventually
from fixtures import EnvironmentVariable
from testtools.matchers import Equals, NotEquals, GreaterThan

from datetime import datetime

from ubuntu_system_settings import SystemSettings

ACCOUNTS_IFACE = 'org.freedesktop.Accounts'
ACCOUNTS_USER_IFACE = 'org.freedesktop.Accounts.User'
ACCOUNTS_OBJ = '/org/freedesktop/Accounts'
MODEM_IFACE = 'org.ofono.Modem'
CONNMAN_IFACE = 'org.ofono.ConnectionManager'
RDO_IFACE = 'org.ofono.RadioSettings'


class UbuntuSystemSettingsTestCase(
        ubuntuuitoolkit.base.UbuntuUIToolkitAppTestCase):

    """Base class for Ubuntu System Settings."""

    def setUp(self, panel=None):
        super(UbuntuSystemSettingsTestCase, self).setUp()
        self.system_settings = SystemSettings(self, panel=panel)
        self.assertThat(
            self.system_settings.main_view.visible,
            Eventually(Equals(True)))


class UbuntuSystemSettingsUpowerTestCase(UbuntuSystemSettingsTestCase,
                                         dbusmock.DBusTestCase):
    """Base class for battery tests that mocks Upower"""

    @classmethod
    def setUpClass(cls):
        cls.start_system_bus()
        cls.dbus_con = cls.get_dbus(True)
        # Add a mock Upower environment so we get consistent results
        (cls.p_mock, cls.obj_upower) = cls.spawn_server_template(
            'upower', {'OnBattery': True}, stdout=subprocess.PIPE)
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
    """ Class for cellular tests which sets up an Ofono mock """

    modem_powered = True
    technology_preference = 'gsm'

    def mock_connection_manager(self):
        self.modem_0.AddProperty(CONNMAN_IFACE, 'Powered',
                                 self.modem_powered)
        self.modem_0.AddProperty(MODEM_IFACE, 'Serial', '1234567890')

        self.modem_0.AddMethods(
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

    def mock_carriers(self):
        self.dbusmock.AddObject(
            '/ril_0/operator/op2',
            'org.ofono.NetworkOperator',
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
            '/ril_0/operator/op3',
            'org.ofono.NetworkOperator',
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

    def mock_radio_settings(self):
        modem_interfaces = self.modem_0.GetProperties()['Interfaces']
        modem_interfaces.append(RDO_IFACE)
        self.modem_0.AddProperty(
            RDO_IFACE, 'TechnologyPreference', self.technology_preference)
        self.modem_0.SetProperty('Interfaces', modem_interfaces)
        self.modem_0.AddMethods(
            RDO_IFACE,
            [
                (
                    'GetProperties', '', 'a{sv}',
                    'ret = self.GetAll("%s")'
                    % RDO_IFACE),
                (
                    'SetProperty', 'sv', '',
                    'self.Set("IFACE", args[0], args[1]); '
                    'self.EmitSignal("IFACE",\
                        "PropertyChanged", "sv", [args[0], args[1]])'.replace(
                        'IFACE', RDO_IFACE)),
            ])

    @classmethod
    def setUpClass(cls):
        cls.start_system_bus()
        cls.dbus_con = cls.get_dbus(True)
        # Add a mock Ofono environment so we get consistent results
        (cls.p_mock, cls.obj_ofono) = cls.spawn_server_template(
            'ofono', stdout=subprocess.PIPE)
        cls.dbusmock = dbus.Interface(cls.obj_ofono, dbusmock.MOCK_IFACE)

    def setUp(self, panel=None):
        self.obj_ofono.Reset()

        # create modem_0 proxy
        self.modem_0 = self.dbus_con.get_object('org.ofono', '/ril_0')

        # Add an available carrier
        self.mock_carriers()

        self.mock_radio_settings()

        self.mock_connection_manager()

        super(UbuntuSystemSettingsOfonoTestCase, self).setUp(panel)

    @classmethod
    def tearDownClass(cls):
        cls.p_mock.terminate()
        cls.p_mock.wait()
        super(UbuntuSystemSettingsOfonoTestCase, cls).tearDownClass()


class CellularBaseTestCase(UbuntuSystemSettingsOfonoTestCase):
    def setUp(self):
        """ Go to Cellular page """
        super(CellularBaseTestCase, self).setUp('cellular')


class AboutBaseTestCase(UbuntuSystemSettingsTestCase):
    def setUp(self):
        """Go to About page."""
        super(AboutBaseTestCase, self).setUp('about')
        self.about_page = self.system_settings.main_view.select_single(
            objectName='aboutPage'
        )


class AboutOfonoBaseTestCase(UbuntuSystemSettingsOfonoTestCase):
    def setUp(self):
        """Go to About page."""
        super(AboutOfonoBaseTestCase, self).setUp('about')
        self.about_page = self.system_settings.main_view.select_single(
            objectName='aboutPage'
        )


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


class SoundBaseTestCase(UbuntuSystemSettingsTestCase):
    """ Base class for sound settings tests"""

    def setUp(self):

        """ Go to Sound page """
        super(SoundBaseTestCase, self).setUp('sound')
        self.assertThat(self.system_settings.main_view.sound_page.active,
                        Eventually(Equals(True)))
