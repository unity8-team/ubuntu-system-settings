# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2013 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

import os
import subprocess
from time import sleep
import unittest

import dbus
from gi.repository import GLib

from autopilot.introspection.dbus import CustomEmulatorBase
from autopilot.matchers import Eventually
from testtools import skipIf
from testtools.matchers import Equals, NotEquals

from ubuntu_system_settings.tests import (
    UbuntuSystemSettingsTestCase,
)
from ubuntu_system_settings.utils.i18n import ugettext as _


class MainPage(CustomEmulatorBase):
    pass


class BackgroundBaseTestCase(UbuntuSystemSettingsTestCase):
    """Base class for Background phone tests """

    def setUp(self):
        """Go to Background page."""
        super(BackgroundBaseTestCase, self).setUp('background')
        self.assertThat(self.background_page.active, Eventually(Equals(True)))

    @property
    def background_page(self):
        """Returns 'Background' page."""
        self.system_settings.main_view.print_tree("/home/phablet/tree.log")
        return self.system_settings.main_view.select_single(
            MainPage,
            objectName='backgroundPage'
        )


class BackgroundTestCase(BackgroundBaseTestCase):
    """Tests for Background this phone Page."""

    def setUp(self):
        super(BackgroundTestCase, self).setUp()

    def _click_welcome_image(self):
        welcome_image = self.background_page.wait_select_single(
            objectName='welcomeImage'
        )
        self.pointer.click_object(welcome_image)
        # FIXME: wait until that page opens pls

    def _click_item(self, item_object_name):
        item = self.background_page.wait_select_single(
            objectName=item_object_name
        )
        self.pointer.click_object(item)

    def test_select_new_welcome_image(self):
        self._click_welcome_image()
        image_filepath = 'file:///home/phablet/.local/share/SystemSettings/Pictures/image20140605_0001.jpg'
        custom_background_image = self.main_view.wait_select_single(
            'QQuickImage',
            source=image_filepath
        )
        self.pointer.click_object(custom_background_image)
        set_button = self.main_view.wait_select_single(
            'Button',
            text='Set'
        )
        self.pointer.click_object(set_button)
        main_page = self.main_view.wait_select_single(
            'MainPage',
            welcomeBackground=image_filepath
        )
