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

    def assert_pagestack_popped(self):
        try:
            self.system_settings.main_view.wait_select_single(
                objectName='entryComponent-reset'),
        except Exception as e:
            self.fail('pageStack was not popped, %s' % e)

    @property
    def _reset_launcher_dialog_button(self):
        return self.system_settings.main_view.reset_page.wait_select_single(
            objectName='resetLauncher')

    @property
    def _reset_launcher_button(self):
        return self.system_settings.main_view.wait_select_single(
            objectName='resetLauncherDialog').wait_select_single(
                objectName='resetLauncherAction')

    @property
    def _factory_reset_dialog_button(self):
        return self.system_settings.main_view.reset_page.wait_select_single(
            objectName='factoryReset')

    @property
    def _factory_reset_button(self):
        return self.system_settings.main_view.wait_select_single(
            objectName='factoryResetDialog').wait_select_single(
                objectName='factoryResetAction')

    def test_reset_page_title_is_correct(self):
        """Checks whether Reset page is available"""
        self.assertThat(
            self.system_settings.main_view.reset_page.title,
            Equals(_('Reset phone')))

    def test_reset_launcher(self):
        gsettings = Gio.Settings.new('com.canonical.Unity.Launcher')
        favs = gsettings.get_value('favorites')

        # make sure we reset launcher favorites even if test fails
        # TODO: use decorator
        try:
            self.reset_launcher()
            # wait for calls to dbus
            sleep(0.5)
            calls = self.user_mock.GetCalls()
            self.assertIn('com.canonical.unity.AccountsService', str(calls))
            self.assert_pagestack_popped()
        except Exception as e:
            raise e
        finally:
            gsettings.set_value('favorites', favs)
            # wait for gsettings
            sleep(1)

    def test_factory_reset(self):
        self.factory_reset()
        # wait for calls to dbus
        sleep(0.5)
        calls = self.sys_mock.GetCalls()
        self.assertIn('FactoryReset', str(calls))
        self.assert_pagestack_popped()

    def _show_factory_reset_dialog(self):
        self.system_settings.main_view.scroll_to_and_click(
            self._factory_reset_dialog_button)

    def _click_factory_reset_button(self):
        self.system_settings.main_view.scroll_to_and_click(
            self._factory_reset_button)

    def factory_reset(self):
        self._show_factory_reset_dialog()
        self._click_factory_reset_button()

    def _show_reset_launcher_dialog(self):
        self.system_settings.main_view.scroll_to_and_click(
            self._reset_launcher_dialog_button)

    def _click_reset_launcher_button(self):
        self.system_settings.main_view.scroll_to_and_click(
            self._reset_launcher_button)

    def reset_launcher(self):
        self._show_reset_launcher_dialog()
        self._click_reset_launcher_button()

    def reset_all_settings(self):
        raise NotImplementedError()
