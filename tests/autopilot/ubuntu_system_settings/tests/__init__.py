# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2013 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

""" Tests for Ubuntu System Settings """

from __future__ import absolute_import

from ubuntu_system_settings.utils.i18n import ugettext as _
from ubuntu_system_settings.emulators import MainWindow
from ubuntu_system_settings.helpers import launch_system_settings

from autopilot.input import Mouse, Touch, Pointer
from autopilot.platform import model
from autopilot.testcase import AutopilotTestCase
from autopilot.matchers import Eventually
from testtools.matchers import Equals, NotEquals, GreaterThan

from ubuntuuitoolkit.base import UbuntuUIToolkitAppTestCase
from ubuntuuitoolkit import emulators as toolkit_emulators

import dbus
import dbusmock
import subprocess
from time import sleep

class UbuntuSystemSettingsTestCase(UbuntuUIToolkitAppTestCase):
    """ Base class for Ubuntu System Settings """
    if model() == 'Desktop':
        scenarios = [ ('with mouse', dict(input_device_class=Mouse)) ]
    else:
        scenarios = [ ('with touch', dict(input_device_class=Touch)) ]

    def setUp(self, panel=None):
        super(UbuntuSystemSettingsTestCase, self).setUp()
        self.app = launch_system_settings(
            self,
            panel=panel,
            emulator_base=toolkit_emulators.UbuntuUIToolkitEmulatorBase)
        self.assertThat(self.main_window.visible, Eventually(Equals(True)))

    @property
    def main_window(self):
        """ Return main view """
        return self.app.select_single(MainWindow)

class UbuntuSystemSettingsUpowerTestCase(UbuntuSystemSettingsTestCase,
                                         dbusmock.DBusTestCase):
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
        self.dbusmock.AddDischargingBattery('mock_BATTERY', 'Battery', 50.0, 10)


class UbuntuSystemSettingsBatteryTestCase(UbuntuSystemSettingsUpowerTestCase):
    """ Base class for tests which rely on the presence of a battery """
    def setUp(self):
        super(UbuntuSystemSettingsBatteryTestCase, self).setUp()
        self.add_mock_battery()
        launch_system_settings(self)
        self.assertThat(self.main_window.visible, Eventually(Equals(True)))


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
        self.dbusmock.AddObject('/ril_0/operator/op2',
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
        self.dbusmock.AddObject('/ril_0/operator/op3',
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


class StorageBaseTestCase(AboutBaseTestCase):
    """ Base class for Storage page tests """

    def setUp(self):
        """ Go to Storage Page """
        super(StorageBaseTestCase, self).setUp()
        # Click on 'Storage' option
        button = self.main_window.about_page.select_single(objectName='storageItem')
        self.assertThat(button, NotEquals(None))
        self.main_window.scroll_to_and_click(button)

    def assert_space_item(self, object_name, text):
        """ Checks whether an space item exists and returns a value """
        item = self.main_window.storage_page.select_single(objectName=object_name)
        self.assertThat(item, NotEquals(None))
        label = item.label # Label
        space = item.value # Disk space (bytes)
        self.assertThat(label, Equals(text))
        # Get item's label
        size_label = item.select_single(objectName='sizeLabel')
        self.assertThat(size_label, NotEquals(None))
        values = size_label.text.split(' ')  # Format: "00.0 (bytes|MB|GB)"
        self.assertThat(len(values), GreaterThan(1))


class LicenseBaseTestCase(AboutBaseTestCase):
    """ Base class for Licenses page tests """

    def setUp(self):
        """ Go to License Page """
        super(LicenseBaseTestCase, self).setUp()
        # Click on 'Software licenses' option
        button = self.main_window.select_single(objectName='licenseItem')
        self.assertThat(button, NotEquals(None))
        self.assertThat(button.text, Equals(_('Software licenses')))
        self.main_window.scroll_to_and_click(button)


class SystemUpdatesBaseTestCase(UbuntuSystemSettingsTestCase):
    """ Base class for SystemUpdates page tests """

    def setUp(self):
        """ Go to SystemUpdates Page """
        super(SystemUpdatesBaseTestCase, self).setUp()
        # Click on 'System Updates' option
        button = self.main_window.select_single(
            objectName='entryComponent-system-update')
        self.assertThat(button, NotEquals(None))
        self.main_window.pointer.move_to_object(button)
        self.main_window.pointer.click()

