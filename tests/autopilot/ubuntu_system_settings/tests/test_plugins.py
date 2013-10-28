# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2013 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

from time import sleep

from autopilot.matchers import Eventually
from testtools.matchers import Contains, Equals, NotEquals, GreaterThan
from unittest import skip

from ubuntu_system_settings.tests import UbuntuSystemSettingsTestCase


class SystemSettingsTestCases(UbuntuSystemSettingsTestCase):
    """ Tests for Ubuntu System Settings """

    def setUp(self):
        super(SystemSettingsTestCases, self).setUp()

    def test_title(self):
        """ Checks whether the main_view title is correct """
        header = self.main_view.select_single(objectName='systemSettingsPage')
        self.assertThat(header, NotEquals(None))
        self.assertThat(header.title, Eventually(Equals('System Settings')))
        
    def test_search(self):
        """ Checks whether the Search box is available """
        search = self.main_view.select_single(objectName='searchTextField')
        self.assertThat(search, NotEquals(None))
        
    def test_network_category(self):
        """ Checks whether the Network category is available """
        category = self.main_view.select_single(objectName='categoryGridNetwork')
        self.assertThat(category, NotEquals(None))
        self.assertThat(category.categoryName, Eventually(Equals('Network')))
        
    def test_personal_category(self):
        """ Checks whether the Personal category is available """
        category = self.main_view.select_single(objectName='categoryGridPersonal')
        self.assertThat(category, NotEquals(None))
        self.assertThat(category.categoryName, Eventually(Equals('Personal')))
 
    def test_system_category(self):
        """ Checks whether the System category is available """
        category = self.main_view.select_single(objectName='categoryGridSystem')
        self.assertThat(category, NotEquals(None))
        self.assertThat(category.categoryName, Eventually(Equals('System')))
 
    def test_wifi_plugin(self):
        """ Checks whether the Wi-Fi plugin is available """
        plugin = self.main_view.select_single(objectName='entryComponentWi-Fi')
        self.assertThat(plugin, NotEquals(None))

    def test_cellular_plugin(self):
        """ Checks whether the Cellunar plugin is available """
        plugin = self.main_view.select_single(objectName='entryComponentCellular')
        self.assertThat(plugin, NotEquals(None))

    def test_bluetooth_plugin(self):
        """ Checks whether the Bluetooth plugin is available """
        plugin = self.main_view.select_single(objectName='entryComponentBluetooth')
        self.assertThat(plugin, NotEquals(None))

    def test_background_plugin(self):
        """ Checks whether the Background plugin is available """
        plugin = self.main_view.select_single(objectName='entryComponentBackground')
        self.assertThat(plugin, NotEquals(None))

    def test_sound_plugin(self):
        """ Checks whether the Sound plugin is available """
        plugin = self.main_view.select_single(objectName='entryComponentSound')
        self.assertThat(plugin, NotEquals(None))

    def test_language_plugin(self):
        """ Checks whether the Language plugin is available """
        plugin = self.main_view.select_single(objectName='entryComponentLanguage & Text')
        self.assertThat(plugin, NotEquals(None))

    def test_accounts_plugin(self):
        """ Checks whether the Accounts plugin is available """
        plugin = self.main_view.select_single(objectName='entryComponentAccounts')
        self.assertThat(plugin, NotEquals(None))

    def test_battery_plugin(self):
        """ Checks whether the Battery plugin is available """
        plugin = self.main_view.select_single(objectName='entryComponentBattery')
        self.assertThat(plugin, NotEquals(None))

    def test_timedate_plugin(self):
        """ Checks whether the Time & Date plugin is available """
        plugin = self.main_view.select_single(objectName='entryComponentTime & Date')
        self.assertThat(plugin, NotEquals(None))

    def test_security_plugin(self):
        """ Checks whether the Security plugin is available """
        plugin = self.main_view.select_single(objectName='entryComponentSecurity & Privacy')
        self.assertThat(plugin, NotEquals(None))

    def test_updates_plugin(self):
        """ Checks whether the Updates plugin is available """
        plugin = self.main_view.select_single(objectName='entryComponentUpdates')
        self.assertThat(plugin, NotEquals(None))

