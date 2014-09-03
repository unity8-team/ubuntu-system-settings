# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2013 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

import dateutil.parser
import os
import subprocess
import unittest

from gi.repository import GLib

from autopilot.matchers import Eventually
from autopilot.platform import model
from testtools import skipIf
from testtools.matchers import Equals, NotEquals

from ubuntu_system_settings.tests import (
    AboutBaseTestCase,
    AboutSystemImageBaseTestCase,
    AboutOfonoBaseTestCase,
    StorageBaseTestCase,
    LicenseBaseTestCase
)
from ubuntu_system_settings.utils.i18n import ugettext as _

import dbus


class AboutTestCase(AboutBaseTestCase):

    """Tests for About this phone Page."""

    def _get_os_name(self):
        os_id = subprocess.check_output(
            ['lsb_release', '-is'], universal_newlines=True)
        os_release = subprocess.check_output(
            ['lsb_release', '-rs'], universal_newlines=True)

        return '{} {}'.format(os_id.strip(), os_release.strip())

    def _get_device_serial_number(self):
        try:
            return subprocess.check_output(
                ['getprop', 'ro.serialno'], universal_newlines=True).strip()
        except OSError:
            # getprop is only available on android hardware.
            return None

    def _get_device_manufacturer_and_model(self):
        if model() == 'Desktop':
            manufacturer = open(
                '/sys/devices/virtual/dmi/id/sys_vendor'
            ).read().strip()
            hw_model = open(
                '/sys/devices/virtual/dmi/id/product_name'
            ).read().strip()
        else:
            manufacturer = subprocess.check_output(
                ['getprop', 'ro.product.manufacturer'],
                universal_newlines=True
            ).strip()
            hw_model = subprocess.check_output(
                ['getprop', 'ro.product.model'],
                universal_newlines=True
            ).strip()

        return '{} {}'.format(manufacturer, hw_model)

    def test_device_with_serial_number_must_display_it(self):
        """Checks whether the UI is showing the correct serial number."""
        device_serial = self._get_device_serial_number()
        if not device_serial:
            self.skipTest('The device has no serial number.')
        else:
            self.assertTrue(self.about_page.is_serial_visible())
            displayed_serial = self.about_page.get_serial()
            self.assertThat(
                displayed_serial, Equals(device_serial))

    def test_device_without_serial_must_not_display_it(self):
        device_serial = self._get_device_serial_number()
        if device_serial:
            self.skipTest('The device has serial number.')
        else:
            self.assertFalse(self.about_page.is_serial_visible())

    def test_settings_show_correct_version_of_the_os(self):
        """Ensure the UI is showing the correct version of the OS."""
        device_os_version = self._get_os_name()
        displayed_os_info = self.about_page.get_os_information()
        # TODO: find a way to check the image build number as well
        # -- om26er 10-03-2014
        self.assertTrue(device_os_version in displayed_os_info)

    @skipIf(subprocess.call(
        ['which', 'getprop'], stdout=subprocess.PIPE) != 0,
        'program "getprop" not found'
    )
    def test_hardware_name(self):
        """Ensure the UI is showing the correct device name."""
        displayed_device_name = self.about_page.get_device_name()
        device_name_from_getprop = self._get_device_manufacturer_and_model()

        self.assertEquals(displayed_device_name, device_name_from_getprop)


class AboutOfonoTestCase(AboutOfonoBaseTestCase):
    def _get_imei_from_dbus(self):
        bus = self.get_dbus(system_bus=True)
        try:
            manager = dbus.Interface(
                bus.get_object('org.ofono', '/'), 'org.ofono.Manager'
            )
        except dbus.exceptions.DBusException:
            # oFono interface not found, probably its a desktop.
            return None

        modems = manager.GetModems()
        for path, properties in modems:
            return properties['Serial'] if 'Serial' in properties else None

    def test_device_with_imei_must_display_it(self):
        """Checks whether the UI is exposing the right IMEI."""
        device_imei = self._get_imei_from_dbus()
        if not device_imei:
            self.skipTest('The device has no imei.')
        else:
            self.assertTrue(self.about_page.is_imei_visible())
            displayed_imei = self.about_page.get_imei()
            self.assertThat(displayed_imei, Equals(device_imei))

    def test_device_without_imei_must_not_display_it(self):
        device_imei = self._get_imei_from_dbus()
        if device_imei:
            self.skipTest('The device has imei.')
        else:
            self.assertFalse(self.about_page.is_imei_visible())

    def test_phone_number(self):
        number = self.system_settings.main_view.about_page.select_single(
            objectName="numberItem"
        )
        self.assertEqual(str(number.value), "123456789")


class AboutOfonoMultiSimTestCase(AboutOfonoBaseTestCase):

    use_sims = 2

    def test_phone_numbers(self):
        number1 = self.system_settings.main_view.about_page.select_single(
            objectName="numberItem1"
        )
        number2 = self.system_settings.main_view.about_page.select_single(
            objectName="numberItem2"
        )
        self.assertEqual(str(number1.value), "123456789")
        self.assertEqual(str(number2.value), "08123")


class AboutSystemImageTestCase(AboutSystemImageBaseTestCase):

    def _get_system_image_iface(self):
        bus = self.get_dbus(system_bus=True)
        service = bus.get_object('com.canonical.SystemImage', '/Service')
        iface = dbus.Interface(service, 'com.canonical.SystemImage')
        return iface.Info()

    def _get_last_updated_date(self):
        info = self._get_system_image_iface()[3]

        if info == 'Unknown':
            return _('Never')
        else:
            return dateutil.parser.parse(info.split()[0]).date()

    def test_last_updated(self):
        """Checks whether Last Updated info is correct."""
        last_updated_date_displayed = \
            dateutil.parser.parse(
                self.about_page.get_last_updated_date()).date()
        self.assertEquals(
            last_updated_date_displayed, self._get_last_updated_date())

    def test_check_for_updates(self):
        """
        Checks whether clicking on Check for Updates brings us
        to the Updates page.
        """
        system_updates_page = self.about_page.go_to_check_for_updates()
        self.assertThat(
            system_updates_page.visible, Eventually(Equals(True)))


class StorageTestCase(StorageBaseTestCase):
    """ Tests for Storage """

    def _get_space_by_directory(self, dir_name):
        if dir_name == 'Music':
            location = GLib.get_user_special_dir(
                GLib.UserDirectory.DIRECTORY_MUSIC
            )
        elif dir_name == 'Videos':
            location = GLib.get_user_special_dir(
                GLib.UserDirectory.DIRECTORY_VIDEOS
            )
        elif dir_name == 'Pictures':
            location = GLib.get_user_special_dir(
                GLib.UserDirectory.DIRECTORY_PICTURES
            )
        else:
            raise ValueError(
                '{} directory not handled by this fuction, you need to enhance'
                ' this function to handle that directory.'.format(dir_name)
            )

        if not os.path.exists(location):
            self.skipTest('glib directory {} does not exist'.format(dir_name))

        output = subprocess.check_output(['du', '--block-size=1', location])
        disk_space = output.split()[len(output.split()) - 2]
        return disk_space

    def test_disk(self):
        """ Checks whether disk item is available """
        disk_item = self.system_settings.main_view.storage_page.select_single(
            objectName='diskItem'
        )
        self.assertThat(disk_item.text, Equals('Total storage'))

    def test_space(self):
        """ Checks whether storage item is available """
        self.assert_space_item('storageItem', _('Free space'))

    def test_space_ubuntu(self):
        """ Checks storage item """
        self.assert_space_item('usedByUbuntuItem', _('Used by Ubuntu'))

    @unittest.skip(
        'Disk calculation can take a while depending on different factors '
        'we dont want to wait for it to calculate.'
    )
    def test_space_used_by_movies(self):
        """ Checks whether space shown to be used by movies is
        correct. """
        movie_space = self._get_space_by_directory('Videos')
        movie_space_in_ui = self.get_storage_space_used_by_category(
            'moviesItem'
        )

        self.assertThat(movie_space_in_ui, Eventually(Equals(movie_space)))

    @unittest.skip(
        'Disk calculation can take a while depending on different factors '
        'we dont want to wait for it to calculate.'
    )
    def test_space_used_by_music(self):
        """ Checks whether space shown to be used by music is
        correct. """
        music_space = self._get_space_by_directory('Music')
        music_space_in_ui = self.get_storage_space_used_by_category(
            'audioItem'
        )

        self.assertThat(music_space_in_ui, Eventually(Equals(music_space)))

    @unittest.skip(
        'Disk calculation can take a while depending on different factors '
        'we dont want to wait for it to calculate.'
    )
    def test_space_used_by_pictures(self):
        """ Checks whether space shown to be used by pictures is
        correct. """
        pictures_space = self._get_space_by_directory('Pictures')
        pictures_space_in_ui = self.get_storage_space_used_by_category(
            'picturesItem'
        )

        self.assertThat(
            pictures_space_in_ui, Eventually(Equals(pictures_space))
        )

    def test_space_other_files(self):
        """ Checks whether space item is available """
        self.assert_space_item('otherFilesItem', _('Other files'))

    def test_space_used_by_apps(self):
        """ Checks whether space item is available """
        self.assert_space_item('usedByAppsItem', _('Used by apps'))

    def test_installed_apps(self):
        """ Checks whether Installed Apps list is available """
        installed_apps_list_view = self.system_settings.main_view.\
            storage_page.select_single(objectName='installedAppsListView')
        self.assertThat(installed_apps_list_view, NotEquals(None))


class LicenseTestCase(LicenseBaseTestCase):

    """Tests for Licenses."""

    def test_open_licenses_page(self):
        """Check whether Storage page is available."""
        # FIXME this is not a good Autopilot tests. It would be better if it
        # opens one of the licenses and checks that it is visible.
        # --elopio - 2014-07-02
        self.assertThat(self.licenses_page.active, Equals(True))
