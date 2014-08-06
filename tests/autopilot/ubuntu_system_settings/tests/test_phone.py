# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2014 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

from __future__ import absolute_import

from autopilot.matchers import Eventually
from testtools.matchers import Equals

from ubuntu_system_settings.tests import PhoneOfonoBaseTestCase


class PhoneTestCase(PhoneOfonoBaseTestCase):
    """Tests for Phone Page"""

    def test_call_fwd(self):
        call_fwd_page = self.phone_page.go_to_call_forwarding()
        call_fwd_page.set_forward("41444424")

        # Check that the forward has been set
        self.assertThat(
            call_fwd_page.get_forwarding,
            Eventually(Equals('41444424')))

    def test_call_waiting(self):
        call_wait = self.phone_page.go_to_call_waiting()
        call_wait.enable_call_waiting()


class PhoneDualSimTestCase(PhoneOfonoBaseTestCase):
    """Tests for Phone Page"""

    use_sims = 2

    def test_call_fwd_sim_1(self):
        call_fwd_page = self.phone_page.go_to_call_forwarding(sim=1)
        call_fwd_page.set_forward("41444424")

        # Check that the forward has been set
        self.assertThat(
            call_fwd_page.get_forwarding,
            Eventually(Equals('41444424')))

    def test_call_fwd_sim_2(self):
        call_fwd_page = self.phone_page.go_to_call_forwarding(sim=2)
        call_fwd_page.set_forward("41444424")

        # Check that the forward has been set
        self.assertThat(
            call_fwd_page.get_forwarding,
            Eventually(Equals('41444424')))


    def test_call_waiting_sim_1(self):
        call_wait = self.phone_page.go_to_call_waiting(sim=1)
        call_wait.enable_call_waiting()


    def test_call_waiting_sim_2(self):
        call_wait = self.phone_page.go_to_call_waiting(sim=2)
        call_wait.enable_call_waiting()

