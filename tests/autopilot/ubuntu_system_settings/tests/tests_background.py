# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2013 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

from ubuntu_system_settings.tests import UbuntuSystemSettingsTestCase
from time import sleep

""" Tests for Ubuntu System Settings """

class BackgroundSettingsTestCases(UbuntuSystemSettingsTestCase):
    """ Tests for Background Settings """

    def setUp(self):
        super(BackgroundSettingsTestCases, self).setUp()

    def test_launch(self):
        self.launch_system_settings()
        sleep(20)
