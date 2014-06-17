# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2014 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

from __future__ import absolute_import

import os

from time import sleep
from autopilot.introspection.dbus import StateNotFoundError
from testtools.matchers import Equals, NotEquals, raises

from ubuntu_system_settings.tests import BackgroundBaseTestCase
from ubuntu_system_settings.utils.i18n import ugettext as _

from ubuntuuitoolkit import emulators as toolkit_emulators

def get_wallpapers_from_grid(grid):
    return grid.select_many('*', objectName='itemImg')

class BackgroundTestCase(BackgroundBaseTestCase):
    """ Tests for Background Page """

    def get_wallpapergrid(self, name):
        """Return a WallpaperGrid with given name, or
        all of them"""
        return self.system_settings.main_view.background_page.\
            select_single('WallpaperGrid', objectName=name)

    def get_wallpapers(self, name=None):
        """Return individual wallpapers (QQuickImage) in given grid,
        or all of them"""
        if name:
            return get_wallpapers_from_grid(self.get_wallpapergrid(name))
        else:
            return self.all_wallpapers

    @property
    def all_wallpapergrids(self):
        """Return all WallpaperGrids"""
        return self.system_settings.main_view.background_page.\
            select_many('WallpaperGrid')

    @property
    def all_wallpapers(self):
        """Return all wallpapers in all grids"""
        wallpapers = []
        for grid in self.all_wallpapergrids:
            wallpapers.extend(get_wallpapers_from_grid(grid))
        return wallpapers

    @property
    def selected_wallpaper(self):
        """Return the currently selected QQuickImage.
        We grab the orange border and travers a bit to get to this image"""
        selected_shape = self.system_settings.main_view.background_page.\
            select_single(
                'UbuntuShape', objectName='SelectedShape', visible=True)

        return selected_shape.get_parent().select_single(
            'QQuickImage', objectName='itemImg')

    def save_wallpaper(self):
        save = self.system_settings.main_view.wait_select_single(
            '*', objectName='saveButton')
        self.system_settings.main_view.scroll_to_and_click(save)

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


    def test_that_the_currently_selected_background_comes_from_dbus(self):
        """Test that background file from dbus is selected in UI"""
        current_file = self.selected_wallpaper.get_properties()['source']

        dbus_file = os.path.realpath(self.user_props['BackgroundFile'])
        dbus_file = 'file://%s' % dbus_file

        self.assertEqual(current_file, dbus_file)

    def test_change_background(self):
        """Test happy path for changing background"""

        # wallpaper source that is selected now
        old = self.selected_wallpaper.get_properties()['source']

        # click a wallpaper that is not selected
        self.system_settings.main_view.pointer.click_object(self.all_wallpapers[3])

        # click set/save
        self.save_wallpaper()

        # the newly selected wallpaper source
        new = self.selected_wallpaper.get_properties()['source']

        # assert that UI is updated
        self.assertNotEqual(new, old)

        # assert that dbus changed
        dbus_value = "file://%s" % self.user_obj.GetBackgroundFile()
        self.assertEqual(dbus_value, new)

