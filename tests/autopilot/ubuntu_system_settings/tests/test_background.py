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

from autopilot.matchers import Eventually
from testtools import skipIf
from testtools.matchers import Equals, NotEquals

from ubuntu_system_settings.tests import (
    UbuntuSystemSettingsTestCase,
)
from ubuntu_system_settings.utils.i18n import ugettext as _



class BackgroundBaseTestCase(UbuntuSystemSettingsTestCase):
    """ Base class for Background this phone tests """

    def setUp(self):
        """Go to Background page."""
        super(BackgroundBaseTestCase, self).setUp('background')
        self.assertThat(self.background_page.active, Eventually(Equals(True)))

    @property
    def background_page(self):
        """Returns 'Background' page."""
        return self.main_view.select_single(objectName='backgroundPage')


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
        photo_image_button = self.main_view.wait_select_single(
            'ActionItem',
            text='Photo/Image'
        )
        self.pointer.click_object(photo_image_button)
        #self._click_item('importImage')
        #self.main_view.print_tree('/home/alesage/Desktop/background.log')
        sleep(10)


#    def test_serial(self):
#        """Checks whether the UI is showing the correct serial number."""
#        item = self.about_page.select_single(objectName='serialItem')
#        serial = self._get_device_serial_number()
#
#        if not serial:
#            self.assertThat(item.visible, Equals(False))
#        else:
#            self.assertThat(
#                item.value, Equals(self._get_device_serial_number())
#            )
#
#    def test_imei_information_is_correct(self):
#        """Checks whether the UI is exposing the right IMEI."""
#        imei_item = self.about_page.wait_select_single(
#            objectName='imeiItem')
#        imei_ofono = self._get_imei_from_dbus()
#
#        if not imei_ofono:
#            self.assertThat(imei_item.visible, Equals(False))
#        else:
#            self.assertEquals(imei_item.value, imei_ofono)
#
#    def test_settings_show_correct_version_of_the_os(self):
#        """Ensure the UI is showing the correct version of the OS."""
#        item = self.about_page.select_single(objectName='osItem')
#        # TODO: find a way to check the image build number as well
#        # -- om26er 10-03-2014
#        self.assertTrue(self._get_os_name() in item.value)
#
#    @skipIf(subprocess.call(
#        ['which', 'getprop'], stdout=subprocess.PIPE) != 0,
#        'program "getprop" not found'
#    )
#    def test_hardware_name(self):
#        """Ensure the UI is showing the correct device name."""
#        device_label = self.about_page.select_single(
#            objectName='deviceLabel'
#            ).text
#        device_name_from_getprop = self._get_device_manufacturer_and_model()
#
#        self.assertEquals(device_label, device_name_from_getprop)
#
#    def test_last_updated(self):
#        """Checks whether Last Updated info is correct."""
#        last_updated = self.about_page.select_single(
#            objectName='lastUpdatedItem'
#            ).value
#
#        self.assertEquals(last_updated, self._get_last_updated_date())
