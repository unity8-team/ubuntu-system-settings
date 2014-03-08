# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2013, 2014 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

""" Autopilot  for Ubuntu System Settings """

from __future__ import absolute_import

import time

from ubuntuuitoolkit import emulators as toolkit_emulators


class SystemSettingsEmulatorException(Exception):
    """Exception raised when there is an error with the emulator."""
    pass


class MainWindow(toolkit_emulators.MainView):
    def click_obj(self, obj):
        """Generic way to click an object

        :param obj: The object to click
        """
        obj.visible.wait_for(True)
        self.pointing_device.click_object(obj)
        return obj

    def _click_entry_component(self, object_name):
        """Click and return an entry component

        :param object_name: the object name to click and return
        """
        return self.click_obj(
            self.wait_select_single('EntryComponent',
                                    objectName=object_name))

    def click_updates(self):
        """Click updates and return the updates page"""
        self._click_entry_component('entryComponent-system-update')
        return self.wait_select_single(PageComponent,
                                       objectName='systemUpdatesPage')


class PageComponent(MainWindow):
    # TODO refactor QT code with page names, so better page object pattern
    #   can be used

    # updates pages
    def click_install_all_button(self):
        """click install all button"""
        button = self._get_install_all_button()
        self.click_obj(button)
        return button

    def get_state(self):
            """Return state of app"""
            # TODO make sure this matches design doc
            # from qml code for system updates page:
            #
            # state SEARCHING
            #    notification visible false
            #    installAllButton visible false
            #    checkForUpdatesArea visible true
            #    updateNotification visiible false
            # state NOUPDATES
            #    updateNotification text i18n.tr("Software is up to date")
            #    updateNotification visible true
            #    updateList visible false
            #    installAllButton visible false
            #    checkForUpdatesArea visible false
            # state SYSTEMUPDATEFAILED
            #    notification text i18n.tr("System update has failed.")
            #    notification progression false
            #    notification visible true
            #    installingImageUpdate visible false
            #    checkForUpdatesArea visible false
            #    updatedNotifiation visible false
            # state UPDATE
            #    notification visible false
            #    updateList visible true
            #    installAllButton visible true
            #    checkForUpdatesArea visible true
            #    updatedNotification visible false

            updateNotification = self._get_update_notification()
            if updateNotification.text == 'Software is up to date':
                return 'NOUPDATES'
            install_all_button = self._get_install_all_button()
            if install_all_button.visible:
                return 'UPDATE'
            notification = self._get_notification()
            if notification.text == 'System update has failed.':
                return 'SYSTEMUPDATEFAILED'
            return 'SEARCHING'

    def wait_for_state(self, state, timeout=60):
        """Wait for expected state

        :returns: state
        :raises: SystemSettingsEmulatorException
        """
        for wait in range(timeout):
            if state == self.get_state():
                return state
            time.sleep(1)

        raise SystemSettingsEmulatorException('State not found before '
                                              'timeout')

    def _get_update_notification(self):
        """Return update notification"""
        return self.wait_select_single(objectName='updateNotification')

    def _get_install_all_button(self):
        """Return install all button"""
        return self.wait_select_single('Button',
                                       objectName='installAllButton')

    def _get_notification(self):
        """Return notification"""
        return self.wait_select_single(objectName='notification')

    def _get_update_list(self):
        """Return update list"""
        return self.wait_select_single(objectName='updateList')

    def _get_check_for_updates_area(self):
        """Return the check for updates area"""
        return self.wait_select_single('QQuickItem',
                                       objectName='checkForUpdatesArea')

    def _get_check_for_updates_label(self):
        """return label for check updates area"""
        area = self.get_check_for_updates_area()
        return area.select_single('Label')


class Dialog(MainWindow):
    # updates page dialog
    # TODO add object names to qml file
    def click_not_now(self):
        """Click not now button"""
        button = self.click_obj(
            self.wait_select_single('Button', text='Not Now'))
        button.wait_for_destroyed()

    def click_install_and_restart(self):
        """Click install and restart button"""
        button = self.click_obj(
            self.wait_select_single('Button', text='Install & Restart'))
        button.wait_for_destroyed()
