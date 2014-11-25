# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2014 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

from time import sleep
from autopilot.matchers import Eventually
from testtools.matchers import Equals, NotEquals

from ubuntu_system_settings.tests import SoundBaseTestCase
from ubuntu_system_settings.utils.i18n import ugettext as _


class SoundTestCase(SoundBaseTestCase):
    """ Tests for Sound Page """

    def test_sound_page(self):
        """ Checks whether Sound page is available """
        self.assertThat(
            self.system_settings.main_view.sound_page,
            NotEquals(None)
        )
        self.assertThat(
            self.system_settings.main_view.sound_page.title,
            Equals(_('Sound'))
        )

    def test_silent_mode_sound_switch(self):
        """ Check that silent_mode is present and clickable"""
        snd = self.system_settings.main_view.sound_page.select_single(
            objectName="silentMode")
        self.system_settings.main_view.scroll_to_and_click(snd)
        sleep(0.2)
        calls = self.dbus_mock_isound.GetCalls()
        self.assertThat(
            calls[0][1],
            Equals('Activate')
        )
        self.assertThat(
            calls[0][2][0],
            Equals('silent-mode')
        )

    def test_call_vibrate_sound_switch(self):
        """ Check that call vibrate is present and clickable"""
        snd = self.system_settings.main_view.sound_page.select_single(
            objectName="callVibrate")
        prev_value = self.obj_test.GetIncomingCallVibrate()
        self.system_settings.main_view.scroll_to_and_click(snd)
        sleep(0.2)
        self.assertNotEqual(
            self.obj_test.GetIncomingCallVibrate(),
            prev_value)

    def test_call_vibrate_silent_mode_sound_switch(self):
        """ Check that call vibrate silent mode is present and clickable"""
        snd = self.system_settings.main_view.sound_page.select_single(
            objectName="callVibrateSilentMode")
        prev_value = self.obj_test.GetIncomingCallVibrateSilentMode()
        self.system_settings.main_view.scroll_to_and_click(snd)
        sleep(0.2)
        self.assertNotEqual(
            self.obj_test.GetIncomingCallVibrateSilentMode(),
            prev_value)

    def test_message_vibrate_sound_switch(self):
        """ Check that message vibrate is present and clickable"""
        snd = self.system_settings.main_view.sound_page.select_single(
            objectName="messageVibrate")
        prev_value = self.obj_test.GetIncomingMessageVibrate()
        self.system_settings.main_view.scroll_to_and_click(snd)
        sleep(0.2)
        self.assertNotEqual(
            self.obj_test.GetIncomingMessageVibrate(),
            prev_value)

    def test_message_vibrate_silent_mode_sound_switch(self):
        """ Check that message vibrate silent mode is present and clickable"""
        snd = self.system_settings.main_view.sound_page.select_single(
            objectName="messageVibrateSilentMode")
        prev_value = self.obj_test.GetIncomingMessageVibrateSilentMode()
        self.system_settings.main_view.scroll_to_and_click(snd)
        sleep(0.2)
        self.assertNotEqual(
            self.obj_test.GetIncomingMessageVibrateSilentMode(),
            prev_value)

    def test_keyboard_sound_switch(self):
        """ Check that keyboard sound is present and clickable"""
        kbd_snd = self.system_settings.main_view.sound_page.select_single(
            objectName="keyboardSoundSwitch")
        current_value = kbd_snd.get_properties()["checked"]
        self.system_settings.main_view.scroll_to_and_click(kbd_snd)
        self.assertThat(
            kbd_snd.get_properties()["checked"], NotEquals(current_value))

    def test_dialpad_sounds_switch(self):
        """ Check that dialpad_sounds is present and clickable"""
        snd = self.system_settings.main_view.sound_page.select_single(
            objectName="dialpadSounds")
        prev_value = self.obj_test.GetDialpadSoundsEnabled()
        self.system_settings.main_view.scroll_to_and_click(snd)
        self.assertThat(
            lambda: self.obj_test.GetDialpadSoundsEnabled(),
            Eventually(NotEquals(prev_value)))
