# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2014 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

from __future__ import absolute_import
from testtools.matchers import Equals
from ubuntu_system_settings.tests import ResetBaseTestCase
from ubuntu_system_settings.utils.i18n import ugettext as _
from time import sleep
from gi.repository import Gio


class ResetTestCase(ResetBaseTestCase):
    """Tests for Reset Page"""

    def test_reset_page_title_is_correct(self):
        """Checks whether Reset page is available"""
        self.assertThat(
            self.reset_page.title,
            Equals(_('Reset phone')))

    def test_reset_launcher(self):
        gsettings = Gio.Settings.new('com.canonical.Unity.Launcher')
        favs = gsettings.get_value('favorites')

        # make sure we reset launcher favorites even if test fails
        # TODO: use decorator
        try:
            self.reset_page.reset_launcher()
            # wait for calls to dbus
            sleep(0.5)
            calls = self.user_mock.GetCalls()
            import pdb; pdb.set_trace()
            self.assertIn('com.canonical.unity.AccountsService', str(calls))
        except Exception as e:
            raise e
        finally:
            gsettings.set_value('favorites', favs)
            # wait for gsettings
            sleep(1)

    def test_factory_reset(self):
        self.reset_page.erase_and_reset_everything()
        # wait for calls to dbus
        sleep(0.5)
        calls = self.sys_mock.GetCalls()
        self.assertIn('FactoryReset', str(calls))
