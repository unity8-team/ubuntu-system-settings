# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2014 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

from __future__ import absolute_import

import dbus

from autopilot.matchers import Eventually
from dbusmock.templates.networkmanager import (
    DEVICE_IFACE, NM80211ApSecurityFlags
)
from testtools.matchers import Equals
from time import sleep
from ubuntu_system_settings.tests import WifiBaseTestCase
from ubuntu_system_settings.utils.i18n import ugettext as _
from unittest import skip


class WifiTestCase(WifiBaseTestCase):
    """Tests for Language Page"""

    def setUp(self):
        super(WifiTestCase, self).setUp()

        self.wifi_page._scroll_to_and_click = \
            self.main_view.scroll_to_and_click

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
            'test_ap',
            password='abcdefgh',)

        # allow backend to set up listeners
        sleep(0.3)

        if dialog:
            dialog.wait_until_destroyed()

    def test_connect_to_eduroam(self):
        if not self.wifi_page.have_wireless():
            self.skipTest('Cannot test wireless since it cannot be enabled')
        self.addCleanup(
            self.wifi_page._set_wireless, self.wifi_page.get_wireless())
        self.wifi_page.enable_wireless()
        self.create_access_point(
            'eduroam', 'eduroam',
            security=NM80211ApSecurityFlags.NM_802_11_AP_SEC_KEY_MGMT_802_1X
        )

        dialog = self.wifi_page.connect_to_hidden_network(
            'eduroam',
            username='student',
            password='abcdefgh',
            security='wpa-ep',
            auth='peap',
            protocol='mschapv2',
        )

        # allow backend to set up listeners
        sleep(0.3)

        if dialog:
            dialog.wait_until_destroyed()

        dev = dbus.Interface(self.dbus_con.get_object(
            'org.freedesktop.NetworkManager', self.device_path),
            'org.freedesktop.DBus.Properties')

        conn_obj = dev.Get(
            'org.freedesktop.NetworkManager.Device', 'AvailableConnections'
        )[0]
        conn = dbus.Interface(self.dbus_con.get_object(
            'org.freedesktop.NetworkManager', conn_obj),
            'org.freedesktop.NetworkManager.Settings.Connection')

        conn_settings = conn.GetSettings()
        wconn = conn_settings['connection']
        w802_11_sec = conn_settings['802-11-wireless-security']
        w802_1x = conn_settings['802-1x']

        self.assertEquals(wconn['type'], '802-11-wireless')
        self.assertEquals(w802_11_sec['key-mgmt'], 'wpa-eap')
        self.assertIn('peap', w802_1x['eap'])
        self.assertEquals(w802_1x['identity'], 'student')
        self.assertEquals(w802_1x['password'], 'abcdefgh')
        self.assertEquals(w802_1x['phase2-auth'], 'mschapv2')

    def test_connect_to_nonexistant_hidden_network(self):
        if not self.wifi_page.have_wireless():
            self.skipTest('Cannot test wireless since it cannot be enabled')
        self.addCleanup(
            self.wifi_page._set_wireless, self.wifi_page.get_wireless())
        self.wifi_page.enable_wireless()
        dialog = self.wifi_page.connect_to_hidden_network(
            'n/a',
            password='abcdefgh',
        )

        # allow backend to set up listeners
        sleep(0.3)

        """Mock a StateChanged signal on the Device, using a likely
        scenario of a not found SSID:
            newState = 120 # NM_DEVICE_STATE_FAILED
            oldState = 0 # does not matter
            reason = 53 # NM_DEVICE_STATE_REASON_SSID_NOT_FOUND

            We manually emit this signal, because the networkmanager mock
            currently does not support this. See [1].

            [1] https://github.com/martinpitt/python-dbusmock/issues/8
        """

        self.device_mock.EmitSignal(
            DEVICE_IFACE, 'StateChanged', 'uuu', [120, 0, 53])

        self.assertThat(
            dialog.text, Eventually(Equals(
                _('The Wi-Fi network could not be found'))))

    @skip('skipped due to %s' % (
        'https://github.com/martinpitt/python-dbusmock/issues/7'))
    def test_connect_to_hidden_network_using_secrets(self):
        if not self.wifi_page.have_wireless():
            self.skipTest('Cannot test wireless since it cannot be enabled')
        self.addCleanup(
            self.wifi_page._set_wireless, self.wifi_page.get_wireless())
        self.wifi_page.enable_wireless()
        dialog = self.wifi_page.connect_to_hidden_network(
            'test_ap', security='wpa', password='abcdefgh',
        )

        # allow backend to set up listeners
        sleep(0.3)

        if dialog:
            dialog.wait_until_destroyed()

    @skip('skipped due to %s' % (
        'https://github.com/martinpitt/python-dbusmock/issues/7'))
    def test_connect_to_hidden_network_using_incorrect_secrets(self):
        if not self.wifi_page.have_wireless():
            self.skipTest('Cannot test wireless since it cannot be enabled')
        self.addCleanup(
            self.wifi_page._set_wireless, self.wifi_page.get_wireless())
        self.wifi_page.enable_wireless()
        dialog = self.wifi_page.connect_to_hidden_network(
            'test_ap',
            security='wpa',
            password='abcdefgh',
        )

        # allow backend to set up listeners
        sleep(0.3)

        self.assertThat(
            dialog.text, Eventually(Equals(
                _('Your authentication details were incorrect'))))

    @skip('networkmanager mock does not yet support deletion of cons')
    def test_connect_to_hidden_network_then_cancel(self):
        if not self.wifi_page.have_wireless():
            self.skipTest('Cannot test wireless since it cannot be enabled')
        self.addCleanup(
            self.wifi_page._set_wireless, self.wifi_page.get_wireless())
        self.wifi_page.enable_wireless()
        dialog = self.wifi_page.connect_to_hidden_network('foo',)

        # allow backend to set up listeners
        sleep(0.3)

        dialog.cancel()

        self.assertThat(
            lambda: len(self.active_connection_mock.GetMethodCalls('Delete')),
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

    def test_remove_previous_network(self):
        access_points = ['Series of Tubes', 'dev/null', 'Mi-Fi',
                         'MonkeySphere']

        for idx, ssid in enumerate(access_points):
            self.create_access_point('Mock_AP%d' % idx, ssid)
            self.dbusmock.AddWiFiConnection(
                self.device_path, 'Mock_Con%d' % idx, ssid, '')

        self.wifi_page.remove_previous_network(access_points[0],)

        self.main_view.go_back()

        # wait for ui to update
        sleep(2)

        self.wifi_page.remove_previous_network(access_points[2],)

        # We cannot make any assertions, because connection deletion
        # is currently not supported.

    def test_handle_wifi_url_with_ssid(self):
        if not self.wifi_page.have_wireless():
            self.skipTest('Cannot test wireless since it cannot be enabled')
        self.addCleanup(
            self.wifi_page._set_wireless, self.wifi_page.get_wireless())

        self.launch(panel='wifi', extra_params=[
            'settings://wifi/?ssid=FooBar'
        ])
