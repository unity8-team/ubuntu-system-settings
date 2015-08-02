# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2014 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

from __future__ import absolute_import

from time import sleep
from ubuntu_system_settings.tests import WifiWithSSIDBaseTestCase


class WifiWithTestSSIDTestCase(WifiWithSSIDBaseTestCase):

    ssid = 'test_ap'

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
