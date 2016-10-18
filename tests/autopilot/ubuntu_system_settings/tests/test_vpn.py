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
    PRIV_IFACE as CTV_PRIV_IFACE, VPN_CONN_OPENVPN_IFACE
)


class VpnAddTestCase(VpnBaseTestCase):

    connectivity_parameters = {}

    def test_add_and_configure_openvpn(self):
        page = self.vpn_page.add_vpn()

        # Wait for length of VpnConnections to become 1
        self.assertThat(
            lambda: len(
                self.ctv_private.Get(CTV_PRIV_IFACE, 'VpnConnections')
            ),
            Eventually(Equals(1))
        )
        conn_path = self.ctv_private.Get(CTV_PRIV_IFACE, 'VpnConnections')[0]
        conn_obj = self.get_vpn_connection_object(conn_path)

        page.set_openvpn_server('vpn.ubuntu.com')
        page.set_openvpn_custom_port('1000')

        page.openvpn_okay()

        self.assertThat(
            lambda: conn_obj.Get(VPN_CONN_OPENVPN_IFACE, 'remote'),
            Eventually(Equals('vpn.ubuntu.com'))
        )

        self.assertThat(
            lambda: conn_obj.Get(VPN_CONN_OPENVPN_IFACE, 'portSet'),
            Eventually(Equals(True))
        )
        self.assertThat(
            lambda: conn_obj.Get(VPN_CONN_OPENVPN_IFACE, 'port'),
            Eventually(Equals(1000))
        )
