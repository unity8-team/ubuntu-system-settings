# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2013 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

""" Tests for Ubuntu System Settings """

from __future__ import absolute_import

from autopilot.input import Mouse, Touch, Pointer
from autopilot.platform import model
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
        scenarios = [('with mouse', dict(input_device_class=Mouse))]
    else:
        scenarios = [('with touch', dict(input_device_class=Touch))]

    def setUp(self, panel=None):
        super(UbuntuSystemSettingsTestCase, self).setUp()
        # make sure the environment language is always english because
        # our tests are heavily relying on strings, need to change that
        # ASAP -- om26er 28-02-14
        self.patch_environment('LC_MESSAGES', 'en_US.UTF-8')
        self.launch_system_settings(panel=panel)
        self.assertThat(self.main_view.visible, Eventually(Equals(True)))

    def launch_system_settings(self, panel=None):
        params = ['/usr/bin/system-settings']
        if (model() != 'Desktop'):
            params.append('--desktop_file_hint=/usr/share/applications/'
                          'ubuntu-system-settings.desktop')

        # Launch to a specific panel
        if panel is not None:
            params.append(panel)

        self.app = self.launch_test_application(
            *params,
            app_type='qt',
            emulator_base=toolkit_emulators.UbuntuUIToolkitEmulatorBase)

    @property
    def main_view(self):
        """ Return main view """
        return self.app.select_single("QQuickView")

    @property
    def pointer(self):
        """ Return pointer """
        return Pointer(self.input_device_class.create())

    def scroll_to(self, obj):
        self.app.select_single(toolkit_emulators.Toolbar).close()
        page = self.main_view.select_single(objectName='systemSettingsPage')
        page_right = page.globalRect[0] + page.globalRect[2]
        page_bottom = page.globalRect[1] + page.globalRect[3]
        page_center_x = int(page_right / 2)
        page_center_y = int(page_bottom / 2)
        while obj.globalRect[1] + obj.height > page_bottom:
            self.pointer.drag(page_center_x, page_center_y,
                              page_center_x, page_center_y - obj.height * 2)
            # avoid a flick
            sleep(0.5)

    def scroll_to_and_click(self, obj):
        self.scroll_to(obj)
        self.pointer.click_object(obj)


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
        self.dbusmock.AddDischargingBattery(
            'mock_BATTERY', 'Battery', 50.0, 10
        )


class UbuntuSystemSettingsBatteryTestCase(UbuntuSystemSettingsUpowerTestCase):
    """ Base class for tests which rely on the presence of a battery """
    def setUp(self):
        super(UbuntuSystemSettingsBatteryTestCase, self).setUp()
        self.add_mock_battery()
        self.launch_system_settings()
        self.assertThat(self.main_view.visible, Eventually(Equals(True)))


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

    @property
    def cellular_page(self):
        """ Returns 'About' page """
        return self.main_view.select_single(objectName='cellularPage')

    @property
    def choose_page(self):
        return self.main_view.select_single(objectName="chooseCarrierPage")


class AboutBaseTestCase(UbuntuSystemSettingsTestCase):
    """ Base class for About this phone tests """

    def setUp(self):
        """ Go to About page """
        super(AboutBaseTestCase, self).setUp('about')

    @property
    def about_page(self):
        """ Returns 'About' page """
        return self.main_view.select_single(objectName='aboutPage')


class StorageBaseTestCase(AboutBaseTestCase):
    """ Base class for Storage page tests """

    def setUp(self):
        """ Go to Storage Page """
        super(StorageBaseTestCase, self).setUp()
        # Click on 'Storage' option
        button = self.about_page.select_single(objectName='storageItem')
        self.assertThat(button, NotEquals(None))
        self.scroll_to_and_click(button)

    def assert_space_item(self, object_name, text):
        """ Checks whether an space item exists and returns a value """
        item = self.storage_page.select_single(objectName=object_name)
        self.assertThat(item, NotEquals(None))
        label = item.label  # Label
        self.assertThat(label, Equals(text))
        # Get item's label
        size_label = item.select_single(objectName='sizeLabel')
        self.assertThat(size_label, NotEquals(None))
        values = size_label.text.split(' ')  # Format: "00.0 (bytes|MB|GB)"
        self.assertThat(len(values), GreaterThan(1))

    @property
    def storage_page(self):
        """ Return 'Storage' page """
        return self.main_view.select_single(objectName='storagePage')


class LicenseBaseTestCase(AboutBaseTestCase):
    """ Base class for Licenses page tests """

    def setUp(self):
        """ Go to License Page """
        super(LicenseBaseTestCase, self).setUp()
        # Click on 'Software licenses' option
        button = self.main_view.select_single(objectName='licenseItem')
        self.assertThat(button, NotEquals(None))
        self.assertThat(button.text, Equals('Software licenses'))
        self.scroll_to_and_click(button)

    @property
    def licenses_page(self):
        """ Return 'License' page """
        return self.main_view.select_single(objectName='licensesPage')


class SystemUpdatesBaseTestCase(UbuntuSystemSettingsTestCase):
    """ Base class for SystemUpdates page tests """

    def setUp(self):
        """ Go to SystemUpdates Page """
        super(SystemUpdatesBaseTestCase, self).setUp()
        # Click on 'System Updates' option
        button = self.main_view.select_single(
            objectName='entryComponent-system-update')
        self.assertThat(button, NotEquals(None))
        self.pointer.move_to_object(button)
        self.pointer.click()

    @property
    def updates_page(self):
        """ Return 'System Update' page """
        return self.main_view.select_single(
            objectName='entryComponent-system-update')
