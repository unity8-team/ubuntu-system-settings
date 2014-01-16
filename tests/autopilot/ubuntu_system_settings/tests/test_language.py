# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2013 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

from time import sleep

from autopilot.platform import model
from autopilot.matchers import Eventually
from testtools.matchers import Equals, NotEquals, GreaterThan
from unittest import expectedFailure

from ubuntu_system_settings.tests import LanguageBaseTestCase
from ubuntu_system_settings.utils.i18n import ugettext as _

class LanguageTestCase(LanguageBaseTestCase):
    """ Tests for Language & Text page """

    def test_language_page(self):
        """ Checks whether Language & Text page is available """
        self.assertThat(self.language_page, NotEquals(None))
        self.assertThat(self.language_page.title, Equals(_('Language & Text')))
