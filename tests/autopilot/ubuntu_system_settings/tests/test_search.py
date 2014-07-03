# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
#
# Copyright 2013, 2014 Canonical
#
# This file is part of ubuntu-system-settings
#
# ubuntu-system-settings is free software: you can redistribute it and/or
# modify it under the terms of the GNU General Public License as published by
# the Free Software Foundation; version 3.
#
# ubuntu-system-settings is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

from testtools.matchers import Equals
from autopilot.matchers import Eventually

from ubuntu_system_settings.tests import UbuntuSystemSettingsTestCase
from ubuntu_system_settings.utils.i18n import ugettext as _


""" Tests for Ubuntu System Settings """


class SearchTestCases(UbuntuSystemSettingsTestCase):
    """ Tests for Search """

    def setUp(self):
        super(SearchTestCases, self).setUp()

    def _get_entry_component(self, name):
        return self.system_settings.main_view.wait_select_single(
            objectName='entryComponent-' + name
        )

    def _get_all_entry_components(self):
        return self.system_settings.main_view.select_many(
            'EntryComponent')

    def _type_into_search_box(self, text):
        search_box = self.system_settings.main_view.select_single(
            objectName='searchTextField'
        )
        self.system_settings.main_view.scroll_to_and_click(search_box)
        self.keyboard.type(_(text))
        self.assertThat(search_box.text, Eventually(Equals(text)))

    def test_search_filter_results(self):
        """ Checks whether Search box actually filters the results """
        self._type_into_search_box('Sound')
        sound_icon = self._get_entry_component('sound')

        self.assertThat(sound_icon.visible, Eventually(Equals(True)))
        self.assertEquals(len(self._get_all_entry_components()), 1)

    def test_search_filter_no_matches(self):
        """ Checks that no results are returned if nothing matches """
        self._type_into_search_box('foobar')
        self.assertEquals(len(self._get_all_entry_components()), 0)
