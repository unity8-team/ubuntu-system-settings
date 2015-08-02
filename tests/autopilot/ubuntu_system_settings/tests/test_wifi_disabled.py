# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2014 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

from __future__ import absolute_import


from autopilot.matchers import Eventually
from testtools.matchers import Equals
from ubuntu_system_settings.tests import WifiBaseTestCase


class WifiDisabledTestCase(WifiBaseTestCase):

    indicatornetwork_parameters = {'actions': {
        'wifi.enable': (False, '', [False]),
    }}

    def test_connect_to_hidden_network_dialog_visibility(self):
        self.assertThat(
            lambda: bool(self.wifi_page.select_single(
                '*',
                objectName='connectToHiddenNetwork').visible),
            Eventually(Equals(False)), 'other net dialog not hidden')
