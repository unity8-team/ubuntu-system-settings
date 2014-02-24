# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2013 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

from autopilot.introspection.dbus import StateNotFoundError
from autopilot.matchers import Eventually
from testtools.matchers import Equals, NotEquals, raises

from ubuntu_system_settings.tests import (
    UbuntuSystemSettingsTestCase,
    UbuntuSystemSettingsUpowerTestCase,
    UbuntuSystemSettingsBatteryTestCase
)
from ubuntu_system_settings.utils.i18n import ugettext as _


class SystemSettingsTestCases(UbuntuSystemSettingsTestCase):
    """ Tests for Ubuntu System Settings """

    def setUp(self):
        super(SystemSettingsTestCases, self).setUp()

    def test_title(self):
        """ Checks whether the main_view title is correct """
        header = self.main_view.select_single(objectName='systemSettingsPage')
        self.assertThat(header, NotEquals(None))
        self.assertThat(header.title, Eventually(Equals(_('System Settings'))))

    def test_search(self):
        """ Checks whether the Search box is available """
        search = self.main_view.select_single(objectName='searchTextField')
        self.assertThat(search, NotEquals(None))

    def test_network_category(self):
        """ Checks whether the Network category is available """
        category = self.main_view.select_single(
            objectName='categoryGrid-network'
        )
        self.assertThat(category, NotEquals(None))
        self.assertThat(
            category.categoryName, Eventually(Equals(_('Network')))
        )

    def test_personal_category(self):
        """ Checks whether the Personal category is available """
        category = self.main_view.select_single(
            objectName='categoryGrid-personal'
        )
        self.assertThat(category, NotEquals(None))
        self.assertThat(
            category.categoryName, Eventually(Equals(_('Personal')))
        )

    def test_system_category(self):
        """ Checks whether the System category is available """
        category = self.main_view.select_single(
            objectName='categoryGrid-system'
        )
        self.assertThat(category, NotEquals(None))
        self.assertThat(category.categoryName, Eventually(Equals(_('System'))))

    def test_wifi_plugin(self):
        """ Checks whether the Wi-Fi plugin is available """
        plugin = self.main_view.select_single(objectName='entryComponent-wifi')
        self.assertThat(plugin, NotEquals(None))

    def test_cellular_plugin(self):
        """ Checks whether the Cellunar plugin is available """
        plugin = self.main_view.select_single(
            objectName='entryComponent-cellular'
        )
        self.assertThat(plugin, NotEquals(None))

    def test_bluetooth_plugin(self):
        """ Checks whether the Bluetooth plugin is available """
        plugin = self.main_view.select_single(
            objectName='entryComponent-bluetooth'
        )
        self.assertThat(plugin, NotEquals(None))

    def test_background_plugin(self):
        """ Checks whether the Background plugin is available """
        plugin = self.main_view.select_single(
            objectName='entryComponent-background'
        )
        self.assertThat(plugin, NotEquals(None))

    def test_sound_plugin(self):
        """ Checks whether the Sound plugin is available """
        plugin = self.main_view.select_single(
            objectName='entryComponent-sound'
        )
        self.assertThat(plugin, NotEquals(None))

    def test_language_plugin(self):
        """ Checks whether the Language plugin is available """
        plugin = self.main_view.select_single(
            objectName='entryComponent-language'
        )
        self.assertThat(plugin, NotEquals(None))

    def test_accounts_plugin(self):
        """ Checks whether the Accounts plugin is available """
        plugin = self.main_view.select_single(
            objectName='entryComponent-online-accounts'
        )
        self.assertThat(plugin, NotEquals(None))

    def test_timedate_plugin(self):
        """ Checks whether the Time & Date plugin is available """
        plugin = self.main_view.select_single(
            objectName='entryComponent-time-date'
        )
        self.assertThat(plugin, NotEquals(None))

    def test_security_plugin(self):
        """ Checks whether the Security plugin is available """
        plugin = self.main_view.select_single(
            objectName='entryComponent-security-privacy'
        )
        self.assertThat(plugin, NotEquals(None))

    def test_updates_plugin(self):
        """ Checks whether the Updates plugin is available """
        plugin = self.main_view.select_single(
            objectName='entryComponent-system-update'
        )
        self.assertThat(plugin, NotEquals(None))


class SystemSettingsUpowerTestCases(UbuntuSystemSettingsUpowerTestCase):
    def setUp(self):
        super(SystemSettingsUpowerTestCases, self).setUp()

    def test_no_battery_plugin_without_battery(self):
        """ Checks whether the Battery plugin is not available as we
        have no battery

        """
        self.assertThat(lambda: self.main_view.select_single(
            objectName='entryComponent-battery'),
            raises(StateNotFoundError)
        )

    def test_battery_plugin_battery_hotplugging(self):
        """ Checks whether hotplugging a battery makes the panel visible """
        self.add_mock_battery()
        plugin = self.main_view.select_single(
            objectName='entryComponent-battery'
        )
        self.assertThat(plugin, NotEquals(None))


class SystemSettingsBatteryTestCases(UbuntuSystemSettingsBatteryTestCase):
    def setUp(self):
        super(SystemSettingsBatteryTestCases, self).setUp()

    def test_battery_plugin(self):
        """ checks whether the Battery plugin is available """
        plugin = self.main_view.select_single(
            objectName='entryComponent-battery'
        )
        self.assertThat(plugin, NotEquals(None))
