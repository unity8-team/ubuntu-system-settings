# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2014 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

from __future__ import absolute_import

import dbus

from autopilot.matchers import Eventually
from testtools.matchers import Equals
from time import sleep
from ubuntu_system_settings.tests import (WifiBaseTestCase,
                                          WifiWithSSIDBaseTestCase)
from ubuntu_system_settings.utils.i18n import ugettext as _


class WifiEnabledTestCase(WifiBaseTestCase):
    """Tests for Language Page"""

    connectivity_parameters = {
        'WifiEnabled': True,
    }

    def test_wifi_page_title_is_correct(self):
        """Checks whether Wifi page is available"""
        self.assertThat(
            self.wifi_page.title,
            Equals(_('Wi-Fi')))

    def test_remove_previous_network(self):
        access_points = ['Series of Tubes', 'dev/null', 'Mi-Fi',
                         'MonkeySphere']

        for idx, ssid in enumerate(access_points):
            self.create_access_point('Mock_AP%d' % idx, ssid)
            self.obj_nm.AddWiFiConnection(
                self.device_path, 'Mock_Con%d' % idx, ssid, '',
                dbus.Dictionary(signature='sa{sv}')
            )

        self.wifi_page.remove_previous_network(access_points[0],)

        self.main_view.go_back()

        # wait for ui to update
        sleep(2)

        self.wifi_page.remove_previous_network(access_points[2],)

        # We cannot make any assertions, because connection deletion
        # is currently not supported.


class WifiDisabledTestCase(WifiBaseTestCase):

    connectivity_parameters = {
        'WifiEnabled': False,
    }

    def test_connect_to_hidden_network_dialog_visibility(self):
        self.assertThat(
            lambda: bool(self.wifi_page.select_single(
                '*',
                objectName='connectToHiddenNetwork').visible),
            Eventually(Equals(False)), 'other net dialog not hidden')


class WifiWithTestSSIDTestCase(WifiWithSSIDBaseTestCase):

    ssid = 'test_ap'
    connectivity_parameters = {
        'WifiEnabled': True,
    }

    def test_handle_wifi_url_with_ssid(self):
        dialog = self.main_view.wait_select_single(
            objectName='otherNetworkDialog'
        )
        dialog._scroll_to_and_click = self.main_view.scroll_to_and_click
        dialog.enter_password('abcdefgh')
        dialog.connect()

        # allow backend to set up listeners
        sleep(0.3)

        if dialog:
            dialog.wait_until_destroyed()
