# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2014 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

from autopilot.matchers import Eventually
from testtools.matchers import Equals

from ubuntu_system_settings.tests import VpnBaseTestCase

from ubuntu_system_settings.tests.connectivity import (
    PRIV_IFACE as CTV_PRIV_IFACE
)


class VpnAddTestCase(VpnBaseTestCase):

    connectivity_parameters = {}

    def test_setup(self):
        change_dialog = self.vpn_page.add_vpn()

        # Wait for length of VpnConnections to become 1
        self.assertThat(
            lambda: len(
                self.ctv_private.Get(CTV_PRIV_IFACE, 'VpnConnections')
            ),
            Eventually(Equals(1))
        )

        change_dialog.set_openvpn_server('vpn.ubuntu.com')
        change_dialog.set_openvpn_custom_port("1000")
        change_dialog.openvpn_okay()
