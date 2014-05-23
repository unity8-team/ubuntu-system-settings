# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2014 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

from testtools.matchers import Equals
from autopilot.matchers import Eventually


from ubuntu_system_settings.tests import SoundBaseTestCase
from ubuntu_system_settings import helpers


class SoundTestCase(SoundBaseTestCase):

    def test_ringtone_setting_change_in_ui(self):
        """Ensure ringtone change is shown in UI."""
        ringtone = 'Bliss'
        sounds_list = self.sound_page.open_ringtone_selector()
        sounds_list.choose_ringtone(ringtone)

        sounds_list.go_back_to_sound_page()

        self.assertThat(
            self.sound_page.get_ringtone_setting_button_current_value(),
            Eventually(Equals(ringtone))
        )

    def test_ringtone_setting_change_in_backend(self):
        """Ensure ringtone change saves in backend."""
        ringtone = 'Celestial'
        sounds_list = self.sound_page.open_ringtone_selector()
        current_ringtone = sounds_list.choose_ringtone(ringtone)

        self.assertThat(
            current_ringtone.selected, Eventually(Equals(True))
        )
        self.assertThat(
            lambda: helpers.get_current_ringtone_from_backend().endswith(
                ringtone + '.ogg'), Eventually(Equals(True))
        )
