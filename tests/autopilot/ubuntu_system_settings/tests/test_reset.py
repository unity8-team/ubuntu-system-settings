# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2014 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

from __future__ import absolute_import

from time import sleep

from autopilot.matchers import Eventually
from gi.repository import Gio
from testtools.matchers import Contains, Equals

from ubuntu_system_settings.tests import ResetBaseTestCase
from ubuntu_system_settings.utils.i18n import ugettext as _


class ResetTestCase(ResetBaseTestCase):
    """Tests for Reset Page"""

    def set_unity_launcher_favorites(self, gsettings, favorites):
        gsettings.set_value('favorites', favorites)
        # wait for gsettings
        sleep(1)

    def test_reset_page_title_is_correct(self):
        """Checks whether Reset page is available"""
        self.assertThat(
            self.reset_page.title,
            Equals(_('Reset phone')))

    def test_reset_launcher(self):
        gsettings = Gio.Settings.new('com.canonical.Unity.Launcher')
        favorites = gsettings.get_value('favorites')
        self.addCleanup(
            self.set_unity_launcher_favorites, gsettings, favorites)

        # make sure we reset launcher favorites even if test fails
        # TODO: use decorator
        self.reset_page.reset_launcher()

        self.assertThat(
            lambda: str(self.user_mock.GetCalls()),
            Eventually(Contains('com.canonical.unity.AccountsService')))

    def test_factory_reset(self):
        self.reset_page.erase_and_reset_everything()
        self.assertThat(
            lambda: str(self.sys_mock.GetCalls()),
            Eventually(Contains('FactoryReset')))
