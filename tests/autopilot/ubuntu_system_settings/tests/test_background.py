# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2014 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

from __future__ import absolute_import

import os

from testtools.matchers import Equals

from ubuntu_system_settings.tests import BackgroundBaseTestCase
from ubuntu_system_settings.utils.i18n import ugettext as _

def get_wallpapers_from_grid(grid):
    return grid.select_many(objectName='itemImg')


class BackgroundTestCase(BackgroundBaseTestCase):
    """ Tests for Background Page """

    def setUp(self):
        super(BackgroundTestCase, self).setUp()
        self.background_page = self.system_settings.main_view.background_page

    def get_wallpapergrid(self, name):
        """Return a WallpaperGrid with given name, or
        all of them"""
        return self.select_single('WallpaperGrid', objectName=name)

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
        return self.background_page.select_many('WallpaperGrid')

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
        We grab the orange border and traverse a bit to get to this image"""
        selected_shape = self.background_page.select_single(
            objectName='highLight', visible=True)

        return selected_shape.get_parent().select_single(
            'QQuickImage', objectName='itemImg')

    def save_wallpaper(self):
        """Click on Set/Save button when previewing a wallpaper"""
        save = self.system_settings.main_view.wait_select_single(
            objectName='saveButton')
        self.system_settings.main_view.scroll_to_and_click(save)

    def test_background_page_title_is_correct(self):
        """ Checks whether Background page is available """
        self.assertThat(self.background_page.title, Equals(_('Background')))

    def test_change_background(self):
        """Test happy path for changing background"""

        # wallpaper source that is selected now
        old = self.selected_wallpaper.source

        # click a wallpaper that is not selected
        self.system_settings.main_view.pointer.click_object(
            self.all_wallpapers[3])

        # click set/save
        self.save_wallpaper()

        # the newly selected wallpaper source
        new = self.selected_wallpaper.source

        # assert that UI is updated
        self.assertNotEqual(new, old)

        # assert that dbus changed
        dbus_value = "file://%s" % self.user_proxy.GetBackgroundFile()
        self.assertEqual(dbus_value, new)

    def test_that_the_currently_selected_background_comes_from_dbus(self):
        """Test that background file from dbus is selected in UI"""
        current_file = self.selected_wallpaper.source

        dbus_file = os.path.realpath(self.user_proxy.GetBackgroundFile())
        dbus_file = 'file://%s' % dbus_file

        self.assertEqual(current_file, dbus_file)

    def test_expand_collapse_custom(self):
        """Test that clicking the custom header changes its state"""
        custom = self.background_page.select_single(
            objectName='customArtGrid')
        custom_header = self.background_page.select_single(
            objectName='CustomHeader')

        self.assertEqual(custom.state, 'collapsed')
        self.system_settings.main_view.scroll_to_and_click(custom_header)
        self.assertEqual(custom.state, '')
