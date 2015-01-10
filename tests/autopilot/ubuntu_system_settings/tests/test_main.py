# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2013 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

from autopilot.matchers import Eventually
from gi.repository import Gio, GLib
from testtools.matchers import Equals
from ubuntu_system_settings.tests import UbuntuSystemSettingsTestCase

""" Tests for Ubuntu System Settings """


class MainTestCase(UbuntuSystemSettingsTestCase):
    """ Tests for Search """

    def setUp(self):
        super(MainTestCase, self).setUp()

    def test_enable_rotation_lock(self):
        gsettings = Gio.Settings.new('com.ubuntu.touch.system')
        current = gsettings.get_value('rotation-lock').get_boolean()

        self.addCleanup(
            self.set_orientation, gsettings, GLib.Variant('b', current))

        self.main_view.disable_orientation_lock()
        self.main_view.enable_orientation_lock()

        self.assertThat(
            lambda: gsettings.get_value('rotation-lock').get_boolean(),
            Eventually(Equals(True)))

    def test_disable_rotation_lock(self):
        gsettings = Gio.Settings.new('com.ubuntu.touch.system')
        current = gsettings.get_value('rotation-lock').get_boolean()

        self.addCleanup(
            self.set_orientation, gsettings, GLib.Variant('b', current))

        self.main_view.enable_orientation_lock()
        self.main_view.disable_orientation_lock()

        self.assertThat(
            lambda: gsettings.get_value('rotation-lock').get_boolean(),
            Eventually(Equals(False)))
