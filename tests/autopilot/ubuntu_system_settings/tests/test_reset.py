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


class ResetTestCase(ResetBaseTestCase):
    """Tests for Reset Page"""

    def assert_pagestack_popped(self):
        try:
            self.system_settings.main_view.wait_select_single(objectName='entryComponent-resets'),
        except Exception:
            self.fail("pageStack was not popped (or entryComponent-reset not found)")

    @property
    def reset_launcher(self):
        return self.system_settings.main_view.reset_page.wait_select_single(
            objectName='resetLauncher')

    @property
    def reset_launcher_action(self):
        return self.system_settings.main_view.wait_select_single(
            objectName="resetLauncherDialog").wait_select_single(
                objectName="resetLauncherAction")

    @property
    def factory_reset(self):
        return self.system_settings.main_view.reset_page.wait_select_single(
            objectName='factoryReset')

    @property
    def factory_reset_action(self):
        return self.system_settings.main_view.wait_select_single(
            objectName="factoryResetDialog").wait_select_single(
                objectName="factoryResetAction")

    def test_reset_page_title_is_correct(self):
        """Checks whether Reset page is available"""
        self.assertThat(
            self.system_settings.main_view.reset_page.title,
            Equals(_('Reset phone')))

    def test_reset_launcher(self):
        self.system_settings.main_view.scroll_to_and_click(self.reset_launcher)
        self.system_settings.main_view.scroll_to_and_click(self.reset_launcher_action)

        sleep(0.5)
        calls = self.user_mock.GetCalls()
        self.assertIn('com.canonical.unity.AccountsService', str(calls))
        self.assert_pagestack_popped()

    def test_factory_reset(self):
        self.system_settings.main_view.scroll_to_and_click(self.factory_reset)
        self.system_settings.main_view.scroll_to_and_click(self.factory_reset_action)

        sleep(0.5)
        calls = self.sys_mock.GetCalls()
        self.assertIn('FactoryReset', str(calls))
        self.assert_pagestack_popped()

