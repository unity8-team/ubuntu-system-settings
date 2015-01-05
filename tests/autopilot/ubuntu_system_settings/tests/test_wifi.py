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
from time import sleep
from ubuntu_system_settings.tests import WifiBaseTestCase
from ubuntu_system_settings.utils.i18n import ugettext as _
from unittest import skip


class WifiTestCase(WifiBaseTestCase):
    """Tests for Language Page"""

    def test_wifi_page_title_is_correct(self):
        """Checks whether Wifi page is available"""
        self.assertThat(
            self.wifi_page.title,
            Equals(_('Wi-Fi')))

    def test_connect_to_hidden_network(self):
        if not self.wifi_page.have_wireless():
            self.skipTest('Cannot test wireless since it cannot be enabled')
        self.addCleanup(
            self.wifi_page._set_wireless, self.wifi_page.get_wireless())
        self.wifi_page.enable_wireless()
        dialog = self.wifi_page.connect_to_hidden_network(
            'yeah',
            scroll_to_and_click=self.system_settings.main_view
            .scroll_to_and_click)

        # allow backend to set up listeners
        sleep(0.3)

        """Mock a StateChanged signal on the Device, using a likely
        scenario of a not found SSID:
            newState = 120 # NM_DEVICE_STATE_FAILED
            oldState = 0 # does not matter
            reason = 53 # NM_DEVICE_STATE_REASON_SSID_NOT_FOUND
        """

        self.device_mock.EmitSignal(
            DEVICE_IFACE, 'StateChanged', 'uuu', [100, 0, 0])

        if dialog:
            dialog.wait_until_destroyed()

    def test_connect_to_nonexistant_hidden_network(self):
        if not self.wifi_page.have_wireless():
            self.skipTest('Cannot test wireless since it cannot be enabled')
        self.addCleanup(
            self.wifi_page._set_wireless, self.wifi_page.get_wireless())
        self.wifi_page.enable_wireless()
        dialog = self.wifi_page.connect_to_hidden_network(
            'yeah',
            scroll_to_and_click=self.system_settings.main_view
            .scroll_to_and_click)

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

    @skip('skipped due to bug 1337556')
    def test_connect_to_hidden_network_using_secrets(self):
        if not self.wifi_page.have_wireless():
            self.skipTest('Cannot test wireless since it cannot be enabled')
        self.addCleanup(
            self.wifi_page._set_wireless, self.wifi_page.get_wireless())
        self.wifi_page.enable_wireless()
        dialog = self.wifi_page.connect_to_hidden_network(
            'yeah', security='wpa', password='abcdefgh',
            scroll_to_and_click=self.system_settings.main_view
            .scroll_to_and_click)

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

    @skip('skipped due to bug 1337556')
    def test_connect_to_hidden_network_using_incorrect_secrets(self):
        if not self.wifi_page.have_wireless():
            self.skipTest('Cannot test wireless since it cannot be enabled')
        self.addCleanup(
            self.wifi_page._set_wireless, self.wifi_page.get_wireless())
        self.wifi_page.enable_wireless()
        dialog = self.wifi_page.connect_to_hidden_network(
            'yeah', security='wpa', password='abcdefgh',
            scroll_to_and_click=self.system_settings.main_view
            .scroll_to_and_click)
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
        if not self.wifi_page.have_wireless():
            self.skipTest('Cannot test wireless since it cannot be enabled')
        self.addCleanup(
            self.wifi_page._set_wireless, self.wifi_page.get_wireless())
        self.wifi_page.enable_wireless()
        dialog = self.wifi_page.connect_to_hidden_network(
            'foo',
            scroll_to_and_click=self.system_settings.main_view
            .scroll_to_and_click)

        # allow backend to set up listeners
        sleep(0.3)

        dialog.cancel()

        # check that Disconnect was called once
        self.assertThat(
            lambda:
                len(self.device_mock.GetMethodCalls('Disconnect')),
            Eventually(Equals(1)))

    def test_connect_to_hidden_network_dialog_visibility(self):
        if not self.wifi_page.have_wireless():
            self.skipTest('Cannot test wireless since it cannot be enabled')
        self.addCleanup(
            self.wifi_page._set_wireless, self.wifi_page.get_wireless())
        self.wifi_page.disable_wireless()
        self.assertThat(
            lambda: bool(self.wifi_page.select_single(
                '*',
                objectName='connectToHiddenNetwork').visible),
            Eventually(Equals(False)), 'other net dialog not hidden')

    """Note: this test does not actually remove previous networks from the UI.
    The NetworkManager dbusmock template does not currently support deletion
    of connections."""
    def test_remove_previous_network(self):
        click_method = self.system_settings.main_view.scroll_to_and_click
        previous_networks = [{
            'ssid': 'Series of Tubes',
            'connection_name': 'conn_0'
        }, {
            'ssid': 'Mi-Fi',
            'connection_name': 'conn_1'
        }, {
            'ssid': 'dev/null',
            'connection_name': 'conn_2'
        }]

        self.add_previous_networks(previous_networks)

        self.wifi_page.remove_previous_network(
            previous_networks[0]['ssid'], scroll_to_and_click=click_method)

        self.system_settings.main_view.go_back()

        # wait for ui to update
        sleep(2)
        self.wifi_page.remove_previous_network(
            previous_networks[2]['ssid'], scroll_to_and_click=click_method)
