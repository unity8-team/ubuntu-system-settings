# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2013 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

import os
import subprocess

from autopilot.matchers import Eventually
from autopilot.platform import model
from testtools import skipUnless, skipIf
from testtools.matchers import Equals, NotEquals

from ubuntu_system_settings.tests import (
    AboutBaseTestCase,
    StorageBaseTestCase,
    LicenseBaseTestCase
)
from ubuntu_system_settings.utils.i18n import ugettext as _

import dbus


class AboutTestCase(AboutBaseTestCase):
    """ Tests for About this phone Page """

    def _get_imei_from_dbus(self):
        bus = dbus.SystemBus()
        manager = dbus.Interface(
            bus.get_object('org.ofono', '/'), 'org.ofono.Manager'
        )
        modems = manager.GetModems()

        for path, properties in modems:
            return properties['Serial']

    def _get_os_name(self):
        os_id = subprocess.check_output(['lsb_release', '-is'])
        os_release = subprocess.check_output(['lsb_release', '-rs'])

        return '{} {}'.format(os_id.strip(), os_release.strip())

    def _get_device_serial_number(self):
        return subprocess.check_output(['getprop', 'ro.serialno']).strip()

    def _get_device_manufacturer_and_model(self):
        manufacturer = subprocess.check_output(
            ['getprop', 'ro.product.manufacturer']
            ).strip()
        model = subprocess.check_output(
            ['getprop', 'ro.product.model']
            ).strip()

        return '{} {}'.format(manufacturer, model)

    def _get_system_image_iface(self):
        bus = dbus.SystemBus()
        service = bus.get_object('com.canonical.SystemImage', '/Service')
        iface = dbus.Interface(service, 'com.canonical.SystemImage')
        return iface.Info()

    def _get_last_updated_date(self):
        info = self._get_system_image_iface()[3]

        if info == 'Unknown':
            return _('Never')
        else:
            return str(info.split()[0])

    def test_serial(self):
        """Checks whether the UI is showing the correct serial number."""
        item = self.about_page.select_single(objectName='serialItem')
        
        if model() == 'Desktop':
            self.assertThat(item.value, Equals(_('N/A')))
        else:
            self.assertThat(
                item.value, Equals(self._get_device_serial_number())
            )

    @skipUnless(
        model() == 'Nexus 4',
        'Nexus 4 is currently the only telephony device that we support'
    )
    def test_imei_information_is_correct(self):
        """Checks whether the UI is exposing the right IMEI."""
        imei = self.about_page.wait_select_single(
            objectName='imeiItem').value

        self.assertEquals(imei, self._get_imei_from_dbus())

    def test_settings_show_correct_version_of_the_os(self):
        """Ensure the UI is showing the correct version of the OS."""
        item = self.about_page.select_single(objectName='osItem')
        # TODO: find a way to check the image build number as well
        # -- om26er 10-03-2014
        self.assertTrue(self._get_os_name() in item.value)

    @skipIf(
        model() == 'Desktop',
        'Only run on android devices where getprop is available')
    def test_hardware_name(self):
        """Ensure the UI is showing the correct device name."""
        device_label = self.about_page.select_single(objectName='deviceLabel')
        device_name_from_getprop = self._get_device_manufacturer_and_model()

        self.assertEquals(device_label, device_name_from_getprop)

    @skipIf(
        model() == 'Desktop',
        'Image based upgrades are only available on touch devices, for now.'
    )
    def test_last_updated(self):
        """Checks whether Last Updated info is correct."""
        last_updated = self.about_page.select_single(
            objectName='lastUpdatedItem'
            ).value
        
        self.assertEquals(last_updated, self._get_last_updated_date())


class StorageTestCase(StorageBaseTestCase):
    """ Tests for Storage """

    def _get_space_by_directory(self, dir_name):
        location = os.path.expanduser('~/' + dir_name)
        output = subprocess.check_output(['du', '--block-size=1', location])
        disk_space = output.split()[len(output.split()) - 2]
        return disk_space

    def test_disk(self):
        """ Checks whether disk item is available """
        disk_item = self.storage_page.wait_select_single(objectName='diskItem')
        self.assertThat(disk_item.text, Equals('Total storage'))

    def test_space(self):
        """ Checks whether storage item is available """
        self.assert_space_item('storageItem', _('Free space'))

    def test_space_ubuntu(self):
        """ Checks storage item """
        self.assert_space_item('usedByUbuntuItem', _('Used by Ubuntu'))

    def test_space_used_by_movies(self):
        """ Checks whether space shown to be used by movies is
        correct. """
        movie_space = self._get_space_by_directory('Videos')
        movie_space_in_ui = self.get_storage_space_used_by_category(
            'moviesItem'
        )

        self.assertEquals(movie_space_in_ui, movie_space)

    def test_space_used_by_music(self):
        """ Checks whether space shown to be used by music is
        correct. """
        music_space = self._get_space_by_directory('Music')
        music_space_in_ui = self.get_storage_space_used_by_category(
            'audioItem'
        )

        self.assertEquals(music_space_in_ui, music_space)

    def test_space_used_by_pictures(self):
        """ Checks whether space shown to be used by pictures is
        correct. """
        pictures_space = self._get_space_by_directory('Pictures')
        pictures_space_in_ui = self.get_storage_space_used_by_category(
            'picturesItem'
        )

        self.assertEquals(pictures_space_in_ui, pictures_space)

    def test_space_other_files(self):
        """ Checks whether space item is available """
        self.assert_space_item('otherFilesItem', _('Other files'))

    def test_space_used_by_apps(self):
        """ Checks whether space item is available """
        self.assert_space_item('usedByAppsItem', _('Used by apps'))

    def test_installed_apps(self):
        """ Checks whether Installed Apps list is available """
        installed_apps_list_view = self.storage_page.select_single(
            objectName='installedAppsListView'
        )
        self.assertThat(installed_apps_list_view, NotEquals(None))


class LicenseTestCase(LicenseBaseTestCase):
    """ Tests for Licenses """

    def test_licenses_page(self):
        """ Check whether Storage page is available """
        self.assertThat(self.licenses_page.active, Eventually(Equals(True)))
