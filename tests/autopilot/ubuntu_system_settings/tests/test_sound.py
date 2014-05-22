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

    def test_ringtone_setting_change_ui(self):
        """Ensure ringtone can be change is shown in UI."""
        ringtone = 'Bliss'
        sounds_list, ringtone_item = self.sound_page.click_ringtone_list_item()
        sounds_list.click_ringtone(ringtone)

        self.assertThat(
            ringtone_item.value, Eventually(Equals(ringtone))
        )

    def test_ringtone_setting_change_backend(self):
        """Ensure ringtone change saves in backend."""
        ringtone = 'Celestial'
        sounds_list = self.sound_page.click_ringtone_list_item()[0]
        sounds_list.click_ringtone(ringtone)

        self.assertThat(
            lambda: helpers.get_current_ringtone().endswith(ringtone + '.ogg'),
            Eventually(Equals(True))
        )
