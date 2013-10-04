# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2013 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

from time import sleep

from autopilot.matchers import Eventually
from testtools.matchers import Contains, Equals, NotEquals, GreaterThan

from ubuntu_system_settings.tests import UbuntuSystemSettingsTestCase

""" Tests for Ubuntu System Settings """

class BackgroundSettingsTestCases(UbuntuSystemSettingsTestCase):
    """ Tests for Background Settings """

    def setUp(self):
        super(BackgroundSettingsTestCases, self).setUp()

    def test_title(self):
        """ Checks whether the main_view title is correct """
        header = self.main_view.select_single('Page', visible=True)
        self.assertThat(header, NotEquals(None))
        self.assertThat(header.title, Eventually(Equals('System Settings')))
        
    def test_search(self):
        """ Checks whether the Search box is available """
        search = self.main_view.select_single('TextField', placeholderText='Search')
        self.assertThat(search, NotEquals(None))
        
    def test_categories(self):
        """ Checks whether the Network category is available """
        category = self.main_view.select_single('CategoryGrid', category='network')
        self.assertThat(category, NotEquals(None))
        self.assertThat(category.categoryName, Eventually(Equals('Network')))
        
    def test_personal_category(self):
        """ Checks whether the Personal category is available """
        category = self.main_view.select_single('CategoryGrid', category='personal')
        self.assertThat(category, NotEquals(None))
        self.assertThat(category.categoryName, Eventually(Equals('Personal')))
 
    def test_system_category(self):
        """ Checks whether the System category is available """
        category = self.main_view.select_single('CategoryGrid', category='system')
        self.assertThat(category, NotEquals(None))
        self.assertThat(category.categoryName, Eventually(Equals('System')))
 
    def test_wifi_plugin(self):
        """ Checks whether the Wi-Fi plugin is available """
        plugin = self.main_view.select_single('Label', text='Wi-Fi')
        self.assertThat(plugin, NotEquals(None))

    def test_cellular_plugin(self):
        """ Checks whether the Cellunar plugin is available """
        plugin = self.main_view.select_single('Label', text='Cellular')
        self.assertThat(plugin, NotEquals(None))

    def test_bluetooth_plugin(self):
        """ Checks whether the Bluetooth plugin is available """
        plugin = self.main_view.select_single('Label', text='Bluetooth')
        self.assertThat(plugin, NotEquals(None))

    def test_background_plugin(self):
        """ Checks whether the Background plugin is available """
        plugin = self.main_view.select_single('Label', text='Background')
        self.assertThat(plugin, NotEquals(None))

    def test_sound_plugin(self):
        """ Checks whether the Sound plugin is available """
        plugin = self.main_view.select_single('Label', text='Sound')
        self.assertThat(plugin, NotEquals(None))

    def test_language_plugin(self):
        """ Checks whether the Language plugin is available """
        plugin = self.main_view.select_single('Label', text='Language & Text')
        self.assertThat(plugin, NotEquals(None))

    def test_accounts_plugin(self):
        """ Checks whether the Accounts plugin is available """
        plugin = self.main_view.select_single('Label', text='Accounts')
        self.assertThat(plugin, NotEquals(None))

    def test_battery_plugin(self):
        """ Checks whether the Battery plugin is available """
        plugin = self.main_view.select_single('Label', text='Battery')
        self.assertThat(plugin, NotEquals(None))

    def test_timedate_plugin(self):
        """ Checks whether the Time & Date plugin is available """
        plugin = self.main_view.select_single('Label', text='Time & Date')
        self.assertThat(plugin, NotEquals(None))

    def test_security_plugin(self):
        """ Checks whether the Security plugin is available """
        plugin = self.main_view.select_single('Label', text='Security & Privacy')
        self.assertThat(plugin, NotEquals(None))

    def test_updates_plugin(self):
        """ Checks whether the Updates plugin is available """
        plugin = self.main_view.select_single('Label', text='Updates')
        self.assertThat(plugin, NotEquals(None))


