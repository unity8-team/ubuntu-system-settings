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
import subprocess

import ubuntuuitoolkit
from autopilot.matchers import Eventually
from testtools.matchers import Equals, NotEquals, GreaterThan

from ubuntu_system_settings import SystemSettings


CONNMAN_IFACE = 'org.ofono.ConnectionManager'
RDO_IFACE = 'org.ofono.RadioSettings'
SIM_IFACE = 'org.ofono.SimManager'
NETREG_IFACE = 'org.ofono.NetworkRegistration'


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
    def setUpClass(klass):
        klass.start_system_bus()
        klass.dbus_con = klass.get_dbus(True)
        # Add a mock Upower environment so we get consistent results
        (klass.p_mock, klass.obj_upower) = klass.spawn_server_template(
            'upower', {'OnBattery': True}, stdout=subprocess.PIPE)
        klass.dbusmock = dbus.Interface(klass.obj_upower, dbusmock.MOCK_IFACE)

    def setUp(self, panel=None):
        self.obj_upower.Reset()
        super(UbuntuSystemSettingsUpowerTestCase, self).setUp()

    def add_mock_battery(self):
        """ Make sure we have a battery """
        self.dbusmock.AddDischargingBattery(
            'mock_BATTERY', 'Battery', 50.0, 10
        )


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

    technology_preference = 'gsm'
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
    def get_all_operators(self, name, slow=False):
        return 'ret = [(m, objects[m].GetAll("org.ofono.NetworkOperator")) ' \
               'for m in objects if "%s/operator/" in m]' % name

    def mock_connection_manager(self, modem):
        modem.AddProperty(CONNMAN_IFACE, 'Powered', True)
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
            '/%s/operator/op3' % name,
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

    def mock_radio_settings(self, modem):
        modem_interfaces = modem.GetProperties()['Interfaces']
        modem_interfaces.append(RDO_IFACE)
        modem.AddProperty(
            RDO_IFACE, 'TechnologyPreference', self.technology_preference)
        modem.SetProperty('Interfaces', modem_interfaces)
        modem.AddMethods(
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

    def mock_sim_manager(self, modem, properties=None):
        if not properties:
            properties = {
                'SubscriberNumbers': ['123456', '234567']
            }
        modem_interfaces = modem.GetProperties()['Interfaces']
        modem_interfaces.append(SIM_IFACE)
        modem.AddProperties(SIM_IFACE, properties)
        modem.SetProperty('Interfaces', modem_interfaces)
        modem.AddMethods(
            SIM_IFACE,
            [
                (
                    'GetProperties', '', 'a{sv}',
                    'ret = self.GetAll("%s")'
                    % SIM_IFACE),
                (
                    'SetProperty', 'sv', '',
                    'self.Set("IFACE", args[0], args[1]); '
                    'self.EmitSignal("IFACE",\
                        "PropertyChanged", "sv", [args[0], args[1]])'.replace(
                    'IFACE', SIM_IFACE)),
            ])

    def add_sim1(self):
        # create modem_0 proxy
        self.modem_0 = self.dbus_con.get_object('org.ofono', '/ril_0')

        # Add an available carrier
        self.mock_carriers('ril_0')

        self.mock_radio_settings(self.modem_0)

        self.mock_connection_manager(self.modem_0)

        self.mock_sim_manager(self.modem_0)

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
                self.get_all_operators(second_modem, slow=True)),
        ])
        self.mock_carriers(second_modem)
        self.mock_radio_settings(self.modem_1)
        self.mock_connection_manager(self.modem_1)

        self.mock_sim_manager(self.modem_1, {
            'SubscriberNumbers': ['08123', '938762783']
        })

    @classmethod
    def setUpClass(klass):
        klass.start_system_bus()
        klass.dbus_con = klass.get_dbus(True)
        klass.params = dbus.Dictionary({}, signature='sv')
        # Add a mock Ofono environment so we get consistent results
        (klass.p_mock, klass.obj_ofono) = klass.spawn_server_template(
            'ofono', parameters=klass.params, stdout=subprocess.PIPE)
        klass.dbusmock = dbus.Interface(klass.obj_ofono, dbusmock.MOCK_IFACE)

    def setUp(self, panel=None):
        self.obj_ofono.Reset()

        self.add_sim1()
        if self.use_sims == 2:
            self.add_sim2()

        super(UbuntuSystemSettingsOfonoTestCase, self).setUp('cellular')


class AboutBaseTestCase(UbuntuSystemSettingsTestCase):

    """Base class for About this phone tests."""

    def setUp(self):
        """Go to About page."""
        super(AboutBaseTestCase, self).setUp()
        self.about_page = self.system_settings.main_view.go_to_about_page()


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


class SoundBaseTestCase(UbuntuSystemSettingsTestCase):
    """ Base class for sound settings tests"""

    def setUp(self):
        """ Go to Sound page """
        super(SoundBaseTestCase, self).setUp('sound')
        self.assertThat(self.system_settings.main_view.sound_page.active,
                        Eventually(Equals(True)))
