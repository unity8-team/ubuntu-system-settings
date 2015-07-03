# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2014 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

from __future__ import absolute_import

from autopilot.matchers import Eventually
from testtools.matchers import Contains, Equals

from ubuntu_system_settings.tests import (
    PhoneOfonoBaseTestCase,
    CALL_FWD_IFACE,
    CALL_SETTINGS_IFACE,
    CONNMAN_IFACE
)


class PhoneTestCase(PhoneOfonoBaseTestCase):

    def test_call_fwd(self):
        call_fwd_page = self.phone_page.go_to_call_forwarding()
        call_fwd_page.set_forward('41444424')

        # Check that the forward has been set
        self.assertThat(
            call_fwd_page.current_forwarding,
            Eventually(Equals('41444424')))

        # Check that dbus properties have been updated
        self.assertThat(
            lambda: str(self.modem_0.Get(CALL_FWD_IFACE,
                                         'VoiceUnconditional')),
            Eventually(Contains('41444424')))

    def test_call_waiting(self):
        call_wait = self.phone_page.go_to_call_waiting()
        # we have to help our test here, this normally takes quite a while
        self.modem_0.EmitSignal(
            CALL_SETTINGS_IFACE, 'PropertyChanged', 'sv',
            ['VoiceCallWaiting', 'disabled'])
        call_wait.enable_call_waiting()

        # Check that dbus properties have been updated
        self.assertThat(
            lambda: str(self.modem_0.Get(CALL_SETTINGS_IFACE,
                                         'VoiceCallWaiting')),
            Eventually(Contains('enabled')))

    def test_call_waiting_switch_not_attached(self):
        self.phone_page.go_to_call_waiting()
        self.modem_0.EmitSignal(
            CONNMAN_IFACE, 'PropertyChanged', 'sv',
            ['Attached', 'false'])
        call_wait_switch = self.main_view.wait_select_single(
            objectName='callWaitingSwitch')
        self.assertThat(
            call_wait_switch.enabled,
            Eventually(Equals(False))
        )


class PhoneDualSimTestCase(PhoneOfonoBaseTestCase):

    use_sims = 2

    def test_call_fwd_sim_1(self):
        call_fwd_page = self.phone_page.go_to_call_forwarding(sim=1)
        call_fwd_page.set_forward("41444424")

        # Check that the forward has been set
        self.assertThat(
            call_fwd_page.current_forwarding,
            Eventually(Equals('41444424')))

        # Check that dbus properties have been updated
        self.assertThat(
            lambda: str(self.modem_0.Get(CALL_FWD_IFACE,
                                         'VoiceUnconditional')),
            Eventually(Contains('41444424')))

    def test_call_fwd_sim_2(self):
        call_fwd_page = self.phone_page.go_to_call_forwarding(sim=2)
        call_fwd_page.set_forward('41444424')

        # Check that the forward has been set
        self.assertThat(
            call_fwd_page.current_forwarding,
            Eventually(Equals('41444424')))

        # Check that dbus properties have been updated
        self.assertThat(
            lambda: str(self.modem_1.Get(CALL_FWD_IFACE,
                                         'VoiceUnconditional')),
            Eventually(Contains('41444424')))

    def test_call_waiting_sim_1(self):
        call_wait = self.phone_page.go_to_call_waiting(sim=1)
        # we have to help our test here, this normally takes quite a while
        self.modem_0.EmitSignal(
            CALL_SETTINGS_IFACE, 'PropertyChanged', 'sv',
            ['VoiceCallWaiting', 'disabled'])
        call_wait.enable_call_waiting()

        # Check that dbus properties have been updated
        self.assertThat(
            lambda: str(self.modem_0.Get(CALL_SETTINGS_IFACE,
                                         'VoiceCallWaiting')),
            Eventually(Contains('enabled')))

    def test_call_waiting_sim_2(self):
        call_wait = self.phone_page.go_to_call_waiting(sim=2)
        # we have to help our test here, this normally takes quite a while
        self.modem_1.EmitSignal(
            CALL_SETTINGS_IFACE, 'PropertyChanged', 'sv',
            ['VoiceCallWaiting', 'disabled'])
        call_wait.enable_call_waiting()

        # Check that dbus properties have been updated
        self.assertThat(
            lambda: str(self.modem_1.Get(CALL_SETTINGS_IFACE,
                                         'VoiceCallWaiting')),
            Eventually(Contains('enabled')))

    def test_call_waiting_switch_not_attached_sim_1(self):
        self.phone_page.go_to_call_waiting(sim=1)
        self.modem_0.EmitSignal(
            CONNMAN_IFACE, 'PropertyChanged', 'sv',
            ['Attached', 'false'])
        call_wait_switch = self.main_view.wait_select_single(
            objectName='callWaitingSwitch')
        self.assertThat(
            call_wait_switch.enabled,
            Eventually(Equals(False))
        )

    def test_call_waiting_switch_not_attached_sim_2(self):
        self.phone_page.go_to_call_waiting(sim=2)
        self.modem_1.EmitSignal(
            CONNMAN_IFACE, 'PropertyChanged', 'sv',
            ['Attached', 'false'])
        call_wait_switch = self.main_view.wait_select_single(
            objectName='callWaitingSwitch')
        self.assertThat(
            call_wait_switch.enabled,
            Eventually(Equals(False))
        )
