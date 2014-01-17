# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2013 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

""" Tests for Ubuntu System Settings """

from __future__ import absolute_import

from ubuntu_system_settings.utils.i18n import ugettext as _

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

class UbuntuSystemSettingsTestCase(UbuntuUIToolkitAppTestCase,
                                   dbusmock.DBusTestCase):
    """ Base class for Ubuntu System Settings """
    if model() == 'Desktop':
        scenarios = [ ('with mouse', dict(input_device_class=Mouse)) ]
    else:
        scenarios = [ ('with touch', dict(input_device_class=Touch)) ]

    @classmethod
    def setUpClass(klass):
        klass.start_system_bus()
        klass.dbus_con = klass.get_dbus(True)
        # Add a mock Upower environment so we get consistent results
        (klass.p_mock, klass.obj_upower) = klass.spawn_server_template(
            'upower', {'OnBattery': True}, stdout=subprocess.PIPE)
        klass.dbusmock = dbus.Interface(klass.obj_upower, dbusmock.MOCK_IFACE)

    def setUp(self, panel=None):
        super(UbuntuSystemSettingsTestCase, self).setUp()
        self.obj_upower.Reset()
        self.launch_system_settings(panel=panel)
        self.assertThat(self.main_view.visible, Eventually(Equals(True)))

    def launch_system_settings(self, panel=None):
        params = ['/usr/bin/system-settings']
        if (model() != 'Desktop'):
            params.append('--desktop_file_hint=/usr/share/applications/ubuntu-system-settings.desktop')

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

    def add_mock_battery(self):
        """ Make sure we have a battery """
        self.dbusmock.AddDischargingBattery('mock_BATTERY', 'Battery', 50.0, 10)

    def scroll_to_and_click(self, obj):
        self.app.select_single(toolkit_emulators.Toolbar).close()
        page = self.main_view.select_single(objectName='systemSettingsPage')
        self.pointer.move_to_object(page)
        (page_center_x, page_center_y) = self.pointer.position()
        page_bottom = page.globalRect[1] + page.globalRect[3]
        while obj.globalRect[1] + obj.height > page_bottom:
            self.pointer.move(page_center_x, page_center_y)
            self.pointer.press()
            self.pointer.move(page_center_x, page_center_y - obj.height * 2)
            sleep(0.2)
            self.pointer.release()
        self.pointer.click_object(obj)

class UbuntuSystemSettingsBatteryTestCase(UbuntuSystemSettingsTestCase):
    """ Base class for tests which rely on the presence of a battery """
    def setUp(self):
        super(UbuntuSystemSettingsTestCase, self).setUp()
        self.add_mock_battery()
        self.launch_system_settings()
        self.assertThat(self.main_view.visible, Eventually(Equals(True)))


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
        label = item.label # Label
        space = item.value # Disk space (bytes)
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
        self.assertThat(button.text, Equals(_('Software licenses')))
        self.scroll_to_and_click(button)

    @property
    def licenses_page(self):
        """ Return 'License' page """
        return self.main_view.select_single(objectName='licensesPage')

