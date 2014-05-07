# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2013 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

""" Tests for Ubuntu System Settings """

from __future__ import absolute_import

from ubuntu_system_settings import SystemSettings

from autopilot.input import Mouse, Touch
from autopilot.platform import model
from autopilot.matchers import Eventually
from testtools.matchers import Equals, NotEquals, GreaterThan

from ubuntuuitoolkit.base import UbuntuUIToolkitAppTestCase

import dbus
import dbusmock
import subprocess


class UbuntuSystemSettingsTestCase(UbuntuUIToolkitAppTestCase):
    """ Base class for Ubuntu System Settings """

    if model() == 'Desktop':
        scenarios = [('with mouse', dict(input_device_class=Mouse))]
    else:
        scenarios = [('with touch', dict(input_device_class=Touch))]

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
    """ Class for cellular tests which sets up an Ofono mock """

    @classmethod
    def setUpClass(klass):
        klass.start_system_bus()
        klass.dbus_con = klass.get_dbus(True)
        # Add a mock Ofono environment so we get consistent results
        (klass.p_mock, klass.obj_ofono) = klass.spawn_server_template(
            'ofono', stdout=subprocess.PIPE)
        klass.dbusmock = dbus.Interface(klass.obj_ofono, dbusmock.MOCK_IFACE)

    def setUp(self, panel=None):
        self.obj_ofono.Reset()
        # Add an available carrier
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
        super(UbuntuSystemSettingsOfonoTestCase, self).setUp('cellular')


class AboutBaseTestCase(UbuntuSystemSettingsTestCase):
    """ Base class for About this phone tests """

    def setUp(self):
        """ Go to About page """
        super(AboutBaseTestCase, self).setUp('about')
        self.assertThat(self.about_page.active, Eventually(Equals(True)))


class StorageBaseTestCase(AboutBaseTestCase):
    """ Base class for Storage page tests """

    def setUp(self):
        """ Go to Storage Page """
        super(StorageBaseTestCase, self).setUp()
        # Click on 'Storage' option
        button = self.system_settings.main_view.about_page.select_single(
            'Standard',
            objectName='storageItem'
        )
        self.assertThat(button, NotEquals(None))
        self.system_settings.main_view.scroll_to_and_click(button)
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
            'StorageItem', objectName=objectName
            ).value

    @property
    def storage_page(self):
        """ Return 'Storage' page """
        return self.main_view.select_single(
            'Storage', objectName='storagePage'
        )


class LicenseBaseTestCase(AboutBaseTestCase):
    """ Base class for Licenses page tests """

    def setUp(self):
        """ Go to License Page """
        super(LicenseBaseTestCase, self).setUp()
        # Click on 'Software licenses' option
        button = self.system_settings.main_view.select_single(
            'Standard',
            objectName='licenseItem')
        self.system_settings.main_view.scroll_to_and_click(button)

    @property
    def licenses_page(self):
        """ Return 'License' page """
        return self.main_view.wait_select_single(
            'ItemPage', objectName='licensesPage'
        )


class SystemUpdatesBaseTestCase(UbuntuSystemSettingsTestCase):
    """ Base class for SystemUpdates page tests """

    def setUp(self):
        """ Go to SystemUpdates Page """
        super(SystemUpdatesBaseTestCase, self).setUp()
        # Click on 'System Updates' option
        button = self.system_settings.main_view.select_single(
            objectName='entryComponent-system-update')
        self.assertThat(button, NotEquals(None))
        self.system_settings.main_view.scroll_to_and_click(button)
