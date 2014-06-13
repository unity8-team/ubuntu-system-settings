# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2014 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

from autopilot.introspection.dbus import StateNotFoundError
from testtools.matchers import Equals, NotEquals, raises

from ubuntu_system_settings.tests import BackgroundBaseTestCase
from ubuntu_system_settings.utils.i18n import ugettext as _

from ubuntuuitoolkit import emulators as toolkit_emulators


class BackgroundTestCase(BackgroundBaseTestCase):
    """ Tests for Background Page """

    def select_by_id(self, obj_id):
        return self.system_settings.main_view.background_page.select_single(
            '*',
            id=obj_id
        )

    def test_background_page(self):
        """ Checks whether Background page is available """
        self.assertThat(
            self.system_settings.main_view.background_page,
            NotEquals(None)
        )
        self.assertThat(
            self.system_settings.main_view.background_page.title,
            Equals(_('Background'))
        )


    # def test_can_change_welcomescreen_background(self):
    #     """Test happy path for changing welcome background"""
    #     #self.select_by_id('welcomeImage')
