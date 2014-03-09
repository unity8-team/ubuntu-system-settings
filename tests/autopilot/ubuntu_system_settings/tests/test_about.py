# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2013 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

import subprocess
import os

from autopilot.platform import model
from autopilot.matchers import Eventually
from testtools.matchers import Equals, NotEquals, GreaterThan

from ubuntu_system_settings.tests import (
    AboutBaseTestCase,
    StorageBaseTestCase,
    LicenseBaseTestCase
)


class AboutTestCase(AboutBaseTestCase):
    """ Tests for About this phone Page """

    def test_about_page(self):
        """ Checks whether About page is available """
        self.assertThat(self.about_page, NotEquals(None))
        self.assertThat(self.about_page.title, Equals('About this phone'))

    def test_device(self):
        """ Checks whether Device info is shown """
        device_label = self.about_page.select_single(objectName='deviceLabel')
        self.assertThat(device_label, NotEquals(None))
        self.assertThat(device_label.text, NotEquals(''))

    def test_serial(self):
        """ Checks whether Serial info is available """
        item = self.about_page.select_single(objectName='serialItem')
        self.assertThat(item, NotEquals(None))
        self.assertThat(item.text, Equals('Serial'))
        if (model() == 'Desktop'):
            self.assertThat(item.value, Equals('N/A'))
        else:
            self.assertThat(item.value, NotEquals('N/A'))

    def test_imei(self):
        """ Checks whether IMEI info is available """
        item = self.about_page.select_single(objectName='imeiItem')
        self.assertThat(item, NotEquals(None))
        self.assertThat(item.text, Equals('IMEI'))
        if (model() == 'Desktop'):
            self.assertThat(item.value, Equals('N/A'))
        else:
            self.assertThat(item.value, NotEquals('N/A'))

    def test_software(self):
        """ Checks whether Software info is available """
        item = self.about_page.select_single(objectName='softwareItem')
        self.assertThat(item, NotEquals(None))
        self.assertThat(item.text, Equals('Software:'))

    def test_os(self):
        """ Checks whether OS info is available """
        item = self.about_page.select_single(objectName='osItem')
        self.assertThat(item, NotEquals(None))
        self.assertThat(item.text, Equals('OS'))
        info = item.value.split()
        self.assertThat(len(info), GreaterThan(1))
        self.assertThat(info[0], Equals('Ubuntu'))
        self.assertThat(info[1], NotEquals(''))
        if (len(info) > 2):
            self.assertThat(info[2], NotEquals(''))

    def test_last_updated(self):
        """ Checks whether Last Updated info is available """
        item = self.about_page.select_single(objectName='lastUpdatedItem')
        self.assertThat(item, NotEquals(None))
        self.assertThat(item.text, Equals('Last updated'))
        date = item.value.split('-')
        if (len(date) == 1):
            self.assertThat(item.value, Equals('Never'))
        else:
            # 2013-10-19
            self.assertThat(len(item.value), Equals(10))

    def test_legal(self):
        """ Checks whether Legal info is available """
        item = self.about_page.select_single(objectName='legalItem')
        self.assertThat(item, NotEquals(None))
        self.assertThat(item.text, Equals('Legal:'))

    def test_update(self):
        """ Checks whether Update button is available """
        button = self.about_page.select_single(objectName='updateButton')
        self.assertThat(button, NotEquals(None))
        self.assertThat(button.text, Equals('Check for updates'))


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
        self.assert_space_item('storageItem', 'Free space')

    def test_space_ubuntu(self):
        """ Checks storage item """
        self.assert_space_item('usedByUbuntuItem', 'Used by Ubuntu')

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
        self.assert_space_item('otherFilesItem', 'Other files')

    def test_space_used_by_apps(self):
        """ Checks whether space item is available """
        self.assert_space_item('usedByAppsItem', 'Used by apps')

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
