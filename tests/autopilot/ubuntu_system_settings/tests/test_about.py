# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2013 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

from time import sleep

from autopilot.platform import model
from testtools.matchers import Equals, NotEquals, GreaterThan
from unittest import expectedFailure

from ubuntu_system_settings.tests import (
    AboutBaseTestCase,
    StorageBaseTestCase,
    LicenseBaseTestCase
)
from ubuntu_system_settings.utils.i18n import ugettext as _


class AboutTestCase(AboutBaseTestCase):
    """ Tests for About this phone Page """

    def test_about_page(self):
        """ Checks whether About page is available """
        self.assertThat(self.app.main_view.about_page, NotEquals(None))
        self.assertThat(
            self.app.main_view.about_page.title,
            Equals(_('About this phone'))
        )

    def test_device(self):
        """ Checks whether Device info is shown """
        device_label = self.app.main_view.about_page.select_single(
            objectName='deviceLabel'
        )
        self.assertThat(device_label, NotEquals(None))
        self.assertThat(device_label.text, NotEquals(''))

    def test_serial(self):
        """ Checks whether Serial info is available """
        item = self.app.main_view.about_page.select_single(
            objectName='serialItem'
        )
        self.assertThat(item, NotEquals(None))
        self.assertThat(item.text, Equals(_('Serial')))
        if (model() == 'Desktop'):
            self.assertThat(item.value, Equals(_('N/A')))
        else:
            self.assertThat(item.value, NotEquals(_('N/A')))

    def test_imei(self):
        """ Checks whether IMEI info is available """
        item = self.app.main_view.about_page.select_single(
            objectName='imeiItem'
        )
        self.assertThat(item, NotEquals(None))
        self.assertThat(item.text, Equals(_('IMEI')))
        if (model() == 'Desktop'):
            self.assertThat(item.value, Equals(_('N/A')))
        else:
            self.assertThat(item.value, NotEquals(_('N/A')))

    def test_software(self):
        """ Checks whether Software info is available """
        item = self.app.main_view.about_page.select_single(
            objectName='softwareItem'
        )
        self.assertThat(item, NotEquals(None))
        self.assertThat(item.text, Equals(_('Software:')))

    def test_os(self):
        """ Checks whether OS info is available """
        item = self.app.main_view.about_page.select_single(
            objectName='osItem'
        )
        self.assertThat(item, NotEquals(None))
        self.assertThat(item.text, Equals(_('OS')))
        info = item.value.split()
        self.assertThat(len(info), GreaterThan(1))
        self.assertThat(info[0], Equals(_('Ubuntu')))
        self.assertThat(info[1], NotEquals(''))
        if (len(info) > 2):
            self.assertThat(info[2], NotEquals(''))

    def test_last_updated(self):
        """ Checks whether Last Updated info is available """
        item = self.app.main_view.about_page.select_single(
            objectName='lastUpdatedItem'
        )
        self.assertThat(item, NotEquals(None))
        self.assertThat(item.text, Equals(_('Last updated')))
        date = item.value.split('-')
        if (len(date) == 1):
            self.assertThat(item.value, Equals(_('Never')))
        else:
            # 2013-10-19
            self.assertThat(len(item.value), Equals(10))

    def test_legal(self):
        """ Checks whether Legal info is available """
        item = self.app.main_view.about_page.select_single(
            objectName='legalItem'
        )
        self.assertThat(item, NotEquals(None))
        self.assertThat(item.text, Equals(_('Legal:')))

    def test_update(self):
        """ Checks whether Update button is available """
        button = self.app.main_view.about_page.select_single(
            objectName='updateButton'
        )
        self.assertThat(button, NotEquals(None))
        self.assertThat(button.text, Equals(_('Check for updates')))


class StorageTestCase(StorageBaseTestCase):
    """ Tests for Storage """

    def test_storage_page(self):
        """ Check whether Storage page is available """
        self.assertThat(self.app.main_view.storage_page, NotEquals(None))

    def test_disk(self):
        """ Checks whether disk item is available """
        sleep(5)
        disk_item = self.app.main_view.storage_page.select_single(
            objectName='diskItem'
        )
        self.assertThat(disk_item, NotEquals(None))
        self.assertThat(disk_item.text, Equals(_('Total storage')))

    def test_space(self):
        """ Checks whether storage item is available """
        self.assert_space_item('storageItem', _('Free space'))

    def test_space_ubuntu(self):
        """ Checks storage item """
        self.assert_space_item('usedByUbuntuItem', _('Used by Ubuntu'))

    def test_space_movies(self):
        """ Checks whether space item is available """
        self.assert_space_item('moviesItem', _('Videos'))

    def test_space_audio(self):
        """ Checks whether space item is available """
        self.assert_space_item('audioItem', _('Audio'))

    def test_space_pictures(self):
        """ Checks whether space item is available """
        self.assert_space_item('picturesItem', _('Pictures'))

    def test_space_other_files(self):
        """ Checks whether space item is available """
        self.assert_space_item('otherFilesItem', _('Other files'))

    def test_space_used_by_apps(self):
        """ Checks whether space item is available """
        self.assert_space_item('usedByAppsItem', _('Used by apps'))

    def test_installed_apps(self):
        """ Checks whether Installed Apps list is available """
        installed_apps_list_view = self.app.main_view.storage_page.select_single(
            objectName='installedAppsListView'
        )
        self.assertThat(installed_apps_list_view, NotEquals(None))


class LicenseTestCase(LicenseBaseTestCase):
    """ Tests for Licenses """

    @expectedFailure
    def test_licenses_page(self):
        """ Check whether Storage page is available """
        self.assertThat(self.app.main_view.licenses_page, NotEquals(None))
