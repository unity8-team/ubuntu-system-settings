# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2014 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

from __future__ import absolute_import
from testtools.matchers import Equals
from ubuntu_system_settings.tests import WifiBaseTestCase
from ubuntu_system_settings.utils.i18n import ugettext as _


def connection_failure():
    return "raise dbus.exceptions.DBusException(\
            'org.freedesktop.NetworkManager.Error.ConnectionInvalid',\
            'That aint right')"


class WifiTestCase(WifiBaseTestCase):
    """Tests for Language Page"""

    def test_wifi_page_title_is_correct(self):
        """Checks whether Wifi page is available"""
        self.assertThat(
            self.wifi_page.title,
            Equals(_('Wi-Fi')))

    def test_connect_to_hidden_network(self):
        self.wifi_page.connect_to_hidden_network('Foo')

    def test_connect_to_nonexistant_hidden_network(self):
        self.obj_nm.AddMethod(
            'org.freedesktop.NetworkManager',
            'AddAndActivateConnection', 'a{sa{sv}}oo', 'oo',
            connection_failure())

        self.wifi_page.connect_to_hidden_network('Foo')
        self.assertFalse(True)
