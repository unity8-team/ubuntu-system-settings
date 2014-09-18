# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2014 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

from __future__ import absolute_import
from autopilot.matchers import Eventually
from dbusmock.templates.networkmanager import DEVICE_IFACE
from testtools.matchers import Equals
from ubuntu_system_settings.tests import WifiBaseTestCase
from ubuntu_system_settings.utils.i18n import ugettext as _
from time import sleep


class WifiTestCase(WifiBaseTestCase):
    """Tests for Language Page"""

    def test_wifi_page_title_is_correct(self):
        """Checks whether Wifi page is available"""
        self.assertThat(
            self.wifi_page.title,
            Equals(_('Wi-Fi')))

    def test_connect_to_hidden_network(self):
        self.wifi_page.connect_to_hidden_network('yeah')

        # allow backend to set up listeners
        sleep(0.3)

        """Mock a StateChanged signal on the Device, using a likely
        scenario of a not found SSID:
            newState = 120 # NM_DEVICE_STATE_FAILED
            oldState = 0 # does not matter
            reason = 53 # NM_DEVICE_STATE_REASON_SSID_NOT_FOUND
        """

        dialog = self.device_mock.EmitSignal(
            DEVICE_IFACE, 'StateChanged', 'uuu', [100, 0, 0])

        if dialog:
            dialog.wait_until_destroyed()

    def test_connect_to_nonexistant_hidden_network(self):

        dialog = self.wifi_page.connect_to_hidden_network('yeah')

        # allow backend to set up listeners
        sleep(0.3)

        """Mock a StateChanged signal on the Device, using a likely
        scenario of a not found SSID:
            newState = 120 # NM_DEVICE_STATE_FAILED
            oldState = 0 # does not matter
            reason = 53 # NM_DEVICE_STATE_REASON_SSID_NOT_FOUND
        """

        self.device_mock.EmitSignal(
            DEVICE_IFACE, 'StateChanged', 'uuu', [120, 0, 53])

        self.assertThat(
            dialog.text, Eventually(Equals(
                _('The Wi-Fi network could not be found'))))

    def test_connect_to_hidden_network_using_secrets(self):

        dialog = self.wifi_page.connect_to_hidden_network(
            'yeah', security='wpa', password='abcdefgh')

        # allow backend to set up listeners
        sleep(0.3)

        """Mock a StateChanged signal on the Device, which
        lets the backend know it was the wrong secret:
            newState = 100 # NM_DEVICE_STATE_ACTIVATED
            oldState = 0 # does not matter
            reason = 0 # does not matter
        """

        self.device_mock.EmitSignal(
            DEVICE_IFACE, 'StateChanged', 'uuu', [100, 0, 0])

        if dialog:
            dialog.wait_until_destroyed()

    def test_connect_to_hidden_network_using_incorrect_secrets(self):

        dialog = self.wifi_page.connect_to_hidden_network(
            'yeah', security='wpa', password='abcdefgh')

        # allow backend to set up listeners
        sleep(0.3)

        """Mock a StateChanged signal on the Device, which
        lets the backend know it was the wrong secret:
            newState = 120 # NM_DEVICE_STATE_FAILED
            oldState = 0 # does not matter
            reason = 7 # NM_DEVICE_STATE_REASON_NO_SECRETS
        """

        self.device_mock.EmitSignal(
            DEVICE_IFACE, 'StateChanged', 'uuu', [120, 0, 7])

        self.assertThat(
            dialog.text, Eventually(Equals(
                _('Your authentication details were incorrect'))))

    def test_connect_to_hidden_network_then_cancel(self):

        dialog = self.wifi_page.connect_to_hidden_network('foo')

        # allow backend to set up listeners
        sleep(0.3)

        dialog.cancel()

        # check that Disconnect was called once
        self.assertThat(
            lambda:
                len(self.device_mock.GetMethodCalls('Disconnect')),
            Eventually(Equals(1)))
