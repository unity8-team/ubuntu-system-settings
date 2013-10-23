# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2013 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

""" Tests for Ubuntu System Settings """

from __future__ import absolute_import

from autopilot.input import Pointer
from autopilot.platform import model
from autopilot.testcase import AutopilotTestCase
from autopilot.matchers import Eventually
from testtools.matchers import Equals

from ubuntuuitoolkit.base import UbuntuUIToolkitAppTestCase

class UbuntuSystemSettingsTestCase(UbuntuUIToolkitAppTestCase):
    """ Base class for Ubuntu System Settings """
    def setUp(self):
        super(UbuntuSystemSettingsTestCase, self).setUp()
        self.launch_system_settings()
        self.assertThat(self.main_view.visible, Eventually(Equals(True)))

    def launch_system_settings(self):
        params = ['/usr/bin/system-settings']
        if (model() <> 'Desktop'):
            params.append('--desktop_file_hint=/usr/share/applications/ubuntu-system-settings.desktop')
        self.app = self.launch_test_application(
            *params,
            app_type='qt')

    @property
    def main_view(self):
        """ Return main view """
        return self.app.select_single("QQuickView")
        
    @property
    def pointer(self):
        """ Return pointer """
        return Pointer(self.input_device_class.create())

