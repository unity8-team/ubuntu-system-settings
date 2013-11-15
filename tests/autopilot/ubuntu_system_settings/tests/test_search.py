# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2013 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

from time import sleep

from autopilot.matchers import Eventually
from testtools.matchers import Contains, Equals, NotEquals, GreaterThan
from unittest import skip

from ubuntu_system_settings.tests import UbuntuSystemSettingsTestCase
from ubuntu_system_settings.utils.i18n import ugettext as _


""" Tests for Ubuntu System Settings """

class SearchTestCases(UbuntuSystemSettingsTestCase):
    """ Tests for Search """

    def setUp(self):
        super(SearchTestCases, self).setUp()

    def test_search(self):
        """ Checks whether Search box actually filters the results """
        # Select search text field
        search = self.main_view.select_single(objectName='searchTextField')
        self.assertThat(search, NotEquals(None))
        # Move to text field
        self.pointer.move_to_object(search)
        self.pointer.click()
        # Filter by string
        self.keyboard.type(_('Sound'))
        # Search component
        sound = self.main_view.select_single(objectName='entryComponent-sound')
        self.assertThat(sound, NotEquals(None))
        background = self.main_view.select_single(objectName='entryComponent-background')
        self.assertThat(background, Equals(None))
 
