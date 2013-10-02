# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2013 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

""" Tests for Ubuntu System Settings """

from __future__ import absolute_import

from autopilot.platform import model
from autopilot.testcase import AutopilotTestCase

class UbuntuSystemSettingsTestCase(AutopilotTestCase):
    """ Base class for Ubuntu System Settings """
    def setUp(self):
        super(UbuntuSystemSettingsTestCase, self).setUp()

    def launch_system_settings(self):
        params = ['/usr/bin/system-settings']
        if (model() <> 'Desktop'):
            params.append('--desktop_file_hint=/usr/share/applications/unity8.desktop')
        self.app = self.launch_test_application(
            *params,
            app_type='qt')

