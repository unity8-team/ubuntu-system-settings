# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2013 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

from gi.repository import Gio
from time import sleep
from ubuntu_system_settings.tests import UbuntuSystemSettingsTestCase
from unittest import skip

""" Tests for Ubuntu System Settings """


class MainTestCase(UbuntuSystemSettingsTestCase):
    """ Tests for Search """

    def setUp(self):
        super(MainTestCase, self).setUp()

    @skip("skipped due to rotation lock being hidden")
    def test_enable_rotation_lock(self):
        gsettings = Gio.Settings.new('com.ubuntu.touch.system')
        current = gsettings.get_value('orientation-lock')

        self.addCleanup(
            self.set_orientation, gsettings, current)

        self.system_settings.main_view.disable_orientation_lock()
        self.system_settings.main_view.enable_orientation_lock()

        # wait for gsettings
        sleep(1)

        """
        We cannot determine what the system will set this value to.
        We can just check that it was not set to an empty string, undefined
        or none.
        """
        self.assertNotIn(
            gsettings.get_value('orientation-lock').get_string(),
            ['', 'none', 'undefined'])

    @skip("skipped due to rotation lock being hidden")
    def test_disable_rotation_lock(self):
        gsettings = Gio.Settings.new('com.ubuntu.touch.system')
        current = gsettings.get_value('orientation-lock')

        self.addCleanup(
            self.set_orientation, gsettings, current)

        self.system_settings.main_view.enable_orientation_lock()
        self.system_settings.main_view.disable_orientation_lock()

        # wait for gsettings
        sleep(1)

        self.assertEqual(
            gsettings.get_value('orientation-lock').get_string(),
            'none')
