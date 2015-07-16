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
    SIM_IFACE,
    NETREG_IFACE
)


class PhoneTestCase(PhoneOfonoBaseTestCase):

    def test_call_fwd_unconditional(self):
        # Set busy so we can assert that busy is eventually unset.
        call_fwd_page = self.phone_page.set_forward_on_busy('41444424')

        call_fwd_page = self.phone_page.set_forward_unconditionally('41444424')

        # Check that the forward has been set
        self.assertThat(
            call_fwd_page.get_forward_unconditionally(),
            Eventually(Equals('41444424')))

        # Check that dbus properties have been updated
        self.assertThat(
            lambda: str(self.modem_0.Get(CALL_FWD_IFACE,
                                         'VoiceUnconditional')),
            Eventually(Contains('41444424')))

        # Check that all other forward methods are disabled and unset.
        for method in ['fwdBusy', 'fwdLost', 'fwdUnreachable']:
            method_obj = call_fwd_page.wait_select_single(objectName=method)
            self.assertFalse(method_obj.enabled)

        self.assertEqual('', call_fwd_page.get_forward_on_busy())
        self.assertEqual('', call_fwd_page.get_forward_when_no_answer())
        self.assertEqual('', call_fwd_page.get_forward_when_unreachable())

        call_fwd_page.unset_forward_unconditionally()

        # Check that the forward has been unset
        self.assertThat(
            call_fwd_page.get_forward_unconditionally(),
            Eventually(Equals('')))

        # Check that dbus properties have been updated
        self.assertThat(
            lambda: str(self.modem_0.Get(CALL_FWD_IFACE,
                                         'VoiceUnconditional')),
            Eventually(Contains('')))

    def test_call_fwd_on_busy(self):
        call_fwd_page = self.phone_page.set_forward_on_busy('41444424')

        # Check that the forward has been set
        self.assertThat(
            call_fwd_page.get_forward_on_busy(),
            Eventually(Equals('41444424')))

        # Check that dbus properties have been updated
        self.assertThat(
            lambda: str(self.modem_0.Get(CALL_FWD_IFACE,
                                         'VoiceBusy')),
            Eventually(Contains('41444424')))

        call_fwd_page.unset_forward_on_busy()

        # Check that the forward has been unset
        self.assertThat(
            call_fwd_page.get_forward_on_busy(),
            Eventually(Equals('')))

        # Check that dbus properties have been updated
        self.assertThat(
            lambda: str(self.modem_0.Get(CALL_FWD_IFACE,
                                         'VoiceBusy')),
            Eventually(Contains('')))

    def test_call_fwd_when_no_answer(self):
        call_fwd_page = self.phone_page.set_forward_when_no_answer('41444424')

        # Check that the forward has been set
        self.assertThat(
            call_fwd_page.get_forward_when_no_answer(),
            Eventually(Equals('41444424')))

        # Check that dbus properties have been updated
        self.assertThat(
            lambda: str(self.modem_0.Get(CALL_FWD_IFACE,
                                         'VoiceNoReply')),
            Eventually(Contains('41444424')))

        call_fwd_page.unset_forward_when_no_answer()

        # Check that the forward has been unset
        self.assertThat(
            call_fwd_page.get_forward_when_no_answer(),
            Eventually(Equals('')))

        # Check that dbus properties have been updated
        self.assertThat(
            lambda: str(self.modem_0.Get(CALL_FWD_IFACE,
                                         'VoiceNoReply')),
            Eventually(Contains('')))

    def test_call_fwd_when_unreachable(self):
        call_fwd_page = self.phone_page.set_forward_when_unreachable(
            '41444424'
        )

        # Check that the forward has been set
        self.assertThat(
            call_fwd_page.get_forward_when_unreachable(),
            Eventually(Equals('41444424')))

        # Check that dbus properties have been updated
        self.assertThat(
            lambda: str(self.modem_0.Get(CALL_FWD_IFACE,
                                         'VoiceNotReachable')),
            Eventually(Contains('41444424')))

        call_fwd_page.unset_forward_when_unreachable()

        # Check that the forward has been unset
        self.assertThat(
            call_fwd_page.get_forward_when_unreachable(),
            Eventually(Equals('')))

        # Check that dbus properties have been updated
        self.assertThat(
            lambda: str(self.modem_0.Get(CALL_FWD_IFACE,
                                         'VoiceNotReachable')),
            Eventually(Contains('')))

    def test_call_waiting(self):
        call_wait = self.phone_page._enter_call_waiting()
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
        self.phone_page._enter_call_waiting()
        self.modem_0.EmitSignal(
            NETREG_IFACE, 'PropertyChanged', 'sv',
            ['Status', 'unregistered'])
        call_wait_switch = self.main_view.wait_select_single(
            objectName='callWaitingSwitch')
        self.assertThat(
            call_wait_switch.enabled,
            Eventually(Equals(False))
        )

    # TODO: Test the Services page itself.
    def test_sim_services(self):
        self.assertThat(
            self.phone_page.get_sim_services_enabled(),
            Eventually(Equals(True)))
        self.phone_page.go_to_sim_services()
        self.main_view.go_back()
        self.modem_0.EmitSignal(
            SIM_IFACE, 'PropertyChanged', 'sv',
            ['ServiceNumbers', ''])
        self.assertThat(
            self.phone_page.get_sim_services_enabled(),
            Eventually(Equals(False)))


class PhoneDualSimTestCase(PhoneOfonoBaseTestCase):

    use_sims = 2

    def test_call_fwd_unconditional_sim_1(self):
        # Set busy so we can assert that busy is eventually unset.
        call_fwd_page = self.phone_page.set_forward_on_busy('41444424',
                                                            sim=0)
        call_fwd_page.set_forward_unconditionally('41444424',)

        # Check that the forward has been set
        self.assertThat(
            call_fwd_page.get_forward_unconditionally(),
            Eventually(Equals('41444424')))

        # Check that dbus properties have been updated
        self.assertThat(
            lambda: str(self.modem_0.Get(CALL_FWD_IFACE,
                                         'VoiceUnconditional')),
            Eventually(Contains('41444424')))

        # Check that all other forward methods are disabled and unset.
        for method in ['fwdBusy', 'fwdLost', 'fwdUnreachable']:
            method_obj = call_fwd_page.wait_select_single(objectName=method)
            self.assertFalse(method_obj.enabled)

        self.assertEqual('', call_fwd_page.get_forward_on_busy())
        self.assertEqual('', call_fwd_page.get_forward_when_no_answer())
        self.assertEqual('', call_fwd_page.get_forward_when_unreachable())

        call_fwd_page.unset_forward_unconditionally()

        # Check that the forward has been unset
        self.assertThat(
            call_fwd_page.get_forward_unconditionally(),
            Eventually(Equals('')))

        # Check that dbus properties have been updated
        self.assertThat(
            lambda: str(self.modem_0.Get(CALL_FWD_IFACE,
                                         'VoiceUnconditional')),
            Eventually(Contains('')))

    def test_call_fwd_on_busy_sim_1(self):
        call_fwd_page = self.phone_page.set_forward_on_busy('41444424',
                                                            sim=0)

        # Check that the forward has been set
        self.assertThat(
            call_fwd_page.get_forward_on_busy(),
            Eventually(Equals('41444424')))

        # Check that dbus properties have been updated
        self.assertThat(
            lambda: str(self.modem_0.Get(CALL_FWD_IFACE,
                                         'VoiceBusy')),
            Eventually(Contains('41444424')))

        call_fwd_page.unset_forward_on_busy()

        # Check that the forward has been unset
        self.assertThat(
            call_fwd_page.get_forward_on_busy(),
            Eventually(Equals('')))

        # Check that dbus properties have been updated
        self.assertThat(
            lambda: str(self.modem_0.Get(CALL_FWD_IFACE,
                                         'VoiceBusy')),
            Eventually(Contains('')))

    def test_call_fwd_when_no_answer_sim_1(self):
        call_fwd_page = self.phone_page.set_forward_when_no_answer('41444424',
                                                                   sim=0)

        # Check that the forward has been set
        self.assertThat(
            call_fwd_page.get_forward_when_no_answer(),
            Eventually(Equals('41444424')))

        # Check that dbus properties have been updated
        self.assertThat(
            lambda: str(self.modem_0.Get(CALL_FWD_IFACE,
                                         'VoiceNoReply')),
            Eventually(Contains('41444424')))

        call_fwd_page.unset_forward_when_no_answer()

        # Check that the forward has been unset
        self.assertThat(
            call_fwd_page.get_forward_when_no_answer(),
            Eventually(Equals('')))

        # Check that dbus properties have been updated
        self.assertThat(
            lambda: str(self.modem_0.Get(CALL_FWD_IFACE,
                                         'VoiceNoReply')),
            Eventually(Contains('')))

    def test_call_fwd_when_unreachable_sim_1(self):
        call_fwd_page = self.phone_page.set_forward_when_unreachable(
            '41444424', sim=0
        )

        # Check that the forward has been set
        self.assertThat(
            call_fwd_page.get_forward_when_unreachable(),
            Eventually(Equals('41444424')))

        # Check that dbus properties have been updated
        self.assertThat(
            lambda: str(self.modem_0.Get(CALL_FWD_IFACE,
                                         'VoiceNotReachable')),
            Eventually(Contains('41444424')))

        call_fwd_page.unset_forward_when_unreachable()

        # Check that the forward has been unset
        self.assertThat(
            call_fwd_page.get_forward_when_unreachable(),
            Eventually(Equals('')))

        # Check that dbus properties have been updated
        self.assertThat(
            lambda: str(self.modem_0.Get(CALL_FWD_IFACE,
                                         'VoiceNotReachable')),
            Eventually(Contains('')))

    def test_call_fwd_unconditional_sim_2(self):
        # Set busy so we can assert that busy is eventually unset.
        call_fwd_page = self.phone_page.set_forward_on_busy('41444424',
                                                            sim=1)
        call_fwd_page.set_forward_unconditionally('41444424',)

        # Check that the forward has been set
        self.assertThat(
            call_fwd_page.get_forward_unconditionally(),
            Eventually(Equals('41444424')))

        # Check that dbus properties have been updated
        self.assertThat(
            lambda: str(self.modem_1.Get(CALL_FWD_IFACE,
                                         'VoiceUnconditional')),
            Eventually(Contains('41444424')))

        # Check that all other forward methods are disabled and unset.
        for method in ['fwdBusy', 'fwdLost', 'fwdUnreachable']:
            method_obj = call_fwd_page.wait_select_single(objectName=method)
            self.assertFalse(method_obj.enabled)

        self.assertEqual('', call_fwd_page.get_forward_on_busy())
        self.assertEqual('', call_fwd_page.get_forward_when_no_answer())
        self.assertEqual('', call_fwd_page.get_forward_when_unreachable())

        call_fwd_page.unset_forward_unconditionally()

        # Check that the forward has been unset
        self.assertThat(
            call_fwd_page.get_forward_unconditionally(),
            Eventually(Equals('')))

        # Check that dbus properties have been updated
        self.assertThat(
            lambda: str(self.modem_1.Get(CALL_FWD_IFACE,
                                         'VoiceUnconditional')),
            Eventually(Contains('')))

    def test_call_fwd_on_busy_sim_2(self):
        call_fwd_page = self.phone_page.set_forward_on_busy('41444424',
                                                            sim=1)

        # Check that the forward has been set
        self.assertThat(
            call_fwd_page.get_forward_on_busy(),
            Eventually(Equals('41444424')))

        # Check that dbus properties have been updated
        self.assertThat(
            lambda: str(self.modem_1.Get(CALL_FWD_IFACE,
                                         'VoiceBusy')),
            Eventually(Contains('41444424')))

        call_fwd_page.unset_forward_on_busy()

        # Check that the forward has been unset
        self.assertThat(
            call_fwd_page.get_forward_on_busy(),
            Eventually(Equals('')))

        # Check that dbus properties have been updated
        self.assertThat(
            lambda: str(self.modem_1.Get(CALL_FWD_IFACE,
                                         'VoiceBusy')),
            Eventually(Contains('')))

    def test_call_fwd_when_no_answer_sim_2(self):
        call_fwd_page = self.phone_page.set_forward_when_no_answer('41444424',
                                                                   sim=1)

        # Check that the forward has been set
        self.assertThat(
            call_fwd_page.get_forward_when_no_answer(),
            Eventually(Equals('41444424')))

        # Check that dbus properties have been updated
        self.assertThat(
            lambda: str(self.modem_1.Get(CALL_FWD_IFACE,
                                         'VoiceNoReply')),
            Eventually(Contains('41444424')))

        call_fwd_page.unset_forward_when_no_answer()

        # Check that the forward has been unset
        self.assertThat(
            call_fwd_page.get_forward_when_no_answer(),
            Eventually(Equals('')))

        # Check that dbus properties have been updated
        self.assertThat(
            lambda: str(self.modem_1.Get(CALL_FWD_IFACE,
                                         'VoiceNoReply')),
            Eventually(Contains('')))

    def test_call_fwd_when_unreachable_sim_2(self):
        call_fwd_page = self.phone_page.set_forward_when_unreachable(
            '41444424', sim=1
        )

        # Check that the forward has been set
        self.assertThat(
            call_fwd_page.get_forward_when_unreachable(),
            Eventually(Equals('41444424')))

        # Check that dbus properties have been updated
        self.assertThat(
            lambda: str(self.modem_1.Get(CALL_FWD_IFACE,
                                         'VoiceNotReachable')),
            Eventually(Contains('41444424')))

        call_fwd_page.unset_forward_when_unreachable()

        # Check that the forward has been unset
        self.assertThat(
            call_fwd_page.get_forward_when_unreachable(),
            Eventually(Equals('')))

        # Check that dbus properties have been updated
        self.assertThat(
            lambda: str(self.modem_1.Get(CALL_FWD_IFACE,
                                         'VoiceNotReachable')),
            Eventually(Contains('')))

    def test_call_waiting_sim_1(self):
        call_wait = self.phone_page._enter_call_waiting(sim=0)
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
        call_wait = self.phone_page._enter_call_waiting(sim=1)
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
        self.phone_page._enter_call_waiting(sim=0)
        self.modem_0.EmitSignal(
            NETREG_IFACE, 'PropertyChanged', 'sv',
            ['Status', 'unregistered'])
        call_wait_switch = self.main_view.wait_select_single(
            objectName='callWaitingSwitch')
        self.assertThat(
            call_wait_switch.enabled,
            Eventually(Equals(False))
        )

    def test_call_waiting_switch_not_attached_sim_2(self):
        self.phone_page._enter_call_waiting(sim=1)
        self.modem_1.EmitSignal(
            NETREG_IFACE, 'PropertyChanged', 'sv',
            ['Status', 'unregistered'])
        call_wait_switch = self.main_view.wait_select_single(
            objectName='callWaitingSwitch')
        self.assertThat(
            call_wait_switch.enabled,
            Eventually(Equals(False))
        )

    # TODO: Test the Services page itself.
    def test_sim_services_sim_1(self):
        self.assertThat(
            self.phone_page.get_sim_services_enabled(sim=0),
            Eventually(Equals(True)))
        self.phone_page.go_to_sim_services(sim=0)
        self.main_view.go_back()
        self.modem_0.EmitSignal(
            SIM_IFACE, 'PropertyChanged', 'sv',
            ['ServiceNumbers', ''])
        self.assertThat(
            self.phone_page.get_sim_services_enabled(sim=0),
            Eventually(Equals(False)))

    # TODO: Test the Services page itself.
    def test_sim_services_sim_2(self):
        self.assertThat(
            self.phone_page.get_sim_services_enabled(sim=1),
            Eventually(Equals(True)))
        self.phone_page.go_to_sim_services(sim=1)
        self.main_view.go_back()
        self.modem_1.EmitSignal(
            SIM_IFACE, 'PropertyChanged', 'sv',
            ['ServiceNumbers', ''])
        self.assertThat(
            self.phone_page.get_sim_services_enabled(sim=1),
            Eventually(Equals(False)))
