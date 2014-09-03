# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2014 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

from __future__ import absolute_import

from autopilot.matchers import Eventually
from testtools.matchers import Equals

from ubuntu_system_settings.tests import LanguageBaseTestCase
from ubuntu_system_settings.utils.i18n import ugettext as _


class LanguageTestCase(LanguageBaseTestCase):
    """Tests for Language Page"""

    def test_language_page_title_is_correct(self):
        """Checks whether Language page is available"""
        self.assertThat(
            self.language_page.title,
            Equals(_('Language & Text')))

    def test_change_language(self):
        current_language = self.language_page.get_current_language()
        target_language = current_language + 1
        self.language_page.change_display_language(target_language,
                                                   revert=True)

        self.assertThat(int(self.language_page.get_current_language()),
            Eventually(Equals(int(target_language))))

        self.addCleanup(
            self.change_display_language, current_language)

    def test_change_language_but_revert(self):
        self.language_page.change_display_language(
            self.language_page.get_current_language() + 1, revert=True)
