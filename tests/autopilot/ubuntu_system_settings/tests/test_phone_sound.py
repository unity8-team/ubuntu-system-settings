# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2014 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

from __future__ import absolute_import

from autopilot.matchers import Eventually
from testtools.matchers import NotEquals

from ubuntu_system_settings.tests import SoundBaseTestCase


class PhoneSoundTestCase(SoundBaseTestCase):
    """Tests for Phone Page"""

    def setUp(self):
        super(PhoneSoundTestCase, self).setUp(panel=None)
        self.page = self.main_view.go_to_phone_page()

    def test_dialpad_sounds_switch(self):
        """ Check that dialpad_sounds is present and clickable"""
        prev_value = self.obj_snd.GetDialpadSoundsEnabled()
        if not prev_value:
            self.page.enable_dialpad_sounds()
        else:
            self.page.disable_dialpad_sounds()
        self.assertThat(
            lambda: self.obj_snd.GetDialpadSoundsEnabled(),
            Eventually(NotEquals(prev_value)))
