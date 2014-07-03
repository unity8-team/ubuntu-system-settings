# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
#
# Copyright 2014 Canonical
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation; version 3.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.


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
