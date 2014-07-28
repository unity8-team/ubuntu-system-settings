# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2014 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

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

    def test_keyboard_sound_switch(self):
        """ Check that keyboard sound is present and clickable"""
        kbd_snd = self.system_settings.main_view.sound_page.select_single(
            objectName="keyboardSoundSwitch")
        current_value = kbd_snd.get_properties()["checked"]
        self.system_settings.main_view.pointing_device.click_object(kbd_snd)
        self.assertThat(
            kbd_snd.get_properties()["checked"], NotEquals(current_value))

    def test_silent_mode_sound_switch(self):
        """ Check that silent_mode is present and clickable"""
        snd = self.system_settings.main_view.sound_page.select_single(
            objectName="silentMode")
        current_value = snd.get_properties()["checked"]
        self.system_settings.main_view.pointing_device.click_object(snd)
        self.assertThat(
            snd.get_properties()["checked"], NotEquals(current_value))

    def test_silent_mode_warning(self):
        """ Check that silent_mode warning is shown"""
        snd = self.system_settings.main_view.sound_page.select_single(
            objectName="silentMode")
        sndwarn = self.system_settings.main_view.sound_page.select_single(
            objectName="silentModeWarning")
        self.system_settings.main_view.pointing_device.click_object(snd)
        self.assertThat(
            sndwarn.get_properties()["visible"], Equals(True))

    def test_call_vibrate_sound_switch(self):
        """ Check that call vibrate is present and clickable"""
        snd = self.system_settings.main_view.sound_page.select_single(
            objectName="callVibrate")
        current_value = snd.get_properties()["checked"]
        self.system_settings.main_view.pointing_device.click_object(snd)
        self.assertThat(
            snd.get_properties()["checked"], NotEquals(current_value))

    def test_call_vibrate_silent_mode_sound_switch(self):
        """ Check that call vibrate is present and clickable"""
        snd = self.system_settings.main_view.sound_page.select_single(
            objectName="callVibrateSilentMode")
        current_value = snd.get_properties()["checked"]
        self.system_settings.main_view.pointing_device.click_object(snd)
        self.assertThat(
            snd.get_properties()["checked"], NotEquals(current_value))

    def test_message_vibrate_sound_switch(self):
        """ Check that message vibrate is present and clickable"""
        snd = self.system_settings.main_view.sound_page.select_single(
            objectName="messageVibrate")
        current_value = snd.get_properties()["checked"]
        self.system_settings.main_view.pointing_device.click_object(snd)
        self.assertThat(
            snd.get_properties()["checked"], NotEquals(current_value))

    def test_message_vibrate_silent_mode_sound_switch(self):
        """ Check that call vibrate is present and clickable"""
        snd = self.system_settings.main_view.sound_page.select_single(
            objectName="messageVibrateSilentMode")
        current_value = snd.get_properties()["checked"]
        self.system_settings.main_view.pointing_device.click_object(snd)
        self.assertThat(
            snd.get_properties()["checked"], NotEquals(current_value))
