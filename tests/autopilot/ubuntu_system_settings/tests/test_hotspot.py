# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2014 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

from autopilot.matchers import Eventually
from testtools.matchers import Equals

from ubuntu_system_settings.tests import HotspotBaseTestCase

from ubuntu_system_settings.tests.connectivity import (
    PRIV_IFACE as CTV_PRIV_IFACE, NETS_IFACE as CTV_NETS_IFACE
)


class HotspotNonExistantTestCase(HotspotBaseTestCase):

    connectivity_parameters = {
        'HotspotEnabled': False,
        'HotspotStored': False
    }

    def test_setup(self):
        ssid = 'bar'
        password = 'zomgzomg'
        config = {'ssid': ssid, 'password': password}

        self.hotspot_page.setup_hotspot(config)

        # Assert that the switch is on.
        self.assertTrue(self.hotspot_page.get_hotspot_status())

        self.assertThat(
            lambda: self.ctv_nets.Get(CTV_NETS_IFACE, 'HotspotEnabled'),
            Eventually(Equals(True))
        )

        self.assertThat(
            lambda: bytearray(
                self.ctv_nets.Get(CTV_NETS_IFACE, 'HotspotSsid')
            ).decode('UTF-8'),
            Eventually(Equals(ssid))
        )

        self.assertThat(
            lambda: self.ctv_private.Get(CTV_PRIV_IFACE, 'HotspotPassword'),
            Eventually(Equals(password))
        )

        self.assertThat(
            lambda: self.ctv_nets.Get(CTV_NETS_IFACE, 'HotspotStored'),
            Eventually(Equals(True))
        )


class HotspotExistsTestCase(HotspotBaseTestCase):

    connectivity_parameters = {
        'HotspotStored': True
    }

    def test_enabling(self):
        self.assertThat(
            lambda: self.ctv_nets.Get(CTV_NETS_IFACE, 'HotspotEnabled'),
            Eventually(Equals(False))
        )

        self.hotspot_page.enable_hotspot()

        self.assertThat(
            lambda: self.ctv_nets.Get(CTV_NETS_IFACE, 'HotspotEnabled'),
            Eventually(Equals(True))
        )

    def test_changing(self):
        ssid = 'bar'
        password = 'zomgzomg'
        config = {'ssid': ssid, 'password': password}
        self.hotspot_page.setup_hotspot(config)

        self.assertThat(
            lambda: bytearray(
                self.ctv_nets.Get(CTV_NETS_IFACE, 'HotspotSsid')
            ).decode('UTF-8'),
            Eventually(Equals(ssid))
        )

        self.assertThat(
            lambda: self.ctv_private.Get(CTV_PRIV_IFACE, 'HotspotPassword'),
            Eventually(Equals(password))
        )


class HotspotEnabledTestCase(HotspotBaseTestCase):

    connectivity_parameters = {
        'HotspotStored': True,
        'HotspotEnabled': True
    }

    def test_disabling(self):

        self.assertThat(
            lambda: self.ctv_nets.Get(CTV_NETS_IFACE, 'HotspotEnabled'),
            Eventually(Equals(True))
        )

        self.hotspot_page.disable_hotspot()

        self.assertThat(
            lambda: self.ctv_nets.Get(CTV_NETS_IFACE, 'HotspotEnabled'),
            Eventually(Equals(False))
        )
