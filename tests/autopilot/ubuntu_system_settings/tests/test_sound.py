# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2014 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

from autopilot import platform
from autopilot.matchers import Eventually
from testtools import skipIf
from testtools.matchers import Equals, NotEquals

from ubuntu_system_settings import helpers
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
        self.system_settings.main_view.pointer.click_object(kbd_snd)
        self.assertThat(kbd_snd.get_properties()["checked"], NotEquals(current_value))

    @skipIf(platform.model() == 'Desktop', 'Phones only')
    def test_ringtone_setting_change_in_ui(self):
        """Ensure ringtone change is shown in UI."""
        ringtone = 'Supreme'
        sounds_list = self.sound_page.open_ringtone_selector()
        sounds_list.choose_ringtone(ringtone)

        sounds_list.go_back_to_sound_page()

        self.assertThat(
            self.sound_page.get_ringtone_setting_button_current_value(),
            Eventually(Equals(ringtone))
        )

    @skipIf(platform.model() == 'Desktop', 'Phones only')
    def test_ringtone_setting_change_in_backend(self):
        """Ensure ringtone change saves in backend."""
        ringtone = 'Supreme'
        sounds_list = self.sound_page.open_ringtone_selector()
        current_ringtone = sounds_list.choose_ringtone(ringtone)

        self.assertThat(
            current_ringtone.selected, Eventually(Equals(True))
        )
        self.assertThat(
            lambda: helpers.get_current_ringtone_from_backend().endswith(
                ringtone + '.ogg'), Eventually(Equals(True))
        )
