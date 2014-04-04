# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2013 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

from __future__ import absolute_import

import os

from autopilot.introspection.dbus import StateNotFoundError
from autopilot.matchers import Eventually
from testtools.matchers import Equals, NotEquals, raises

from ubuntu_system_settings.tests import SystemUpdatesBaseTestCase


""" Tests for Ubuntu System Settings """


class SystemUpdatesTestCases(SystemUpdatesBaseTestCase):

    """Tests for System Updates."""

    def setUp(self):
        # Set environment variables
        os.environ["IGNORE_CREDENTIALS"] = "True"
        os.environ["IGNORE_UPDATES"] = "True"
        super(SystemUpdatesTestCases, self).setUp()

    def test_show_updates(self):
        """ Checks whether Search box actually filters the results """
        updates = self.system_settings.main_view.updates_page
        self.assertThat(updates, NotEquals(None))
        # Move to text field
        self.system_settings.main_view.scroll_to_and_click(updates)

    def test_updates_not_in_main(self):
        """Check that the updates notification is shown in main."""
        self.assertThat(lambda: self.system_settings.select_single(
            objectName='entryComponent-updates'), raises(StateNotFoundError))

    def test_configuration(self):
        """Check the configuration button."""
        self.assertThat(lambda: self.system_settings.select_single(
            objectName='configurationPage'), raises(StateNotFoundError))
        updates = self.system_settings.main_view.updates_page
        self.assertThat(updates, NotEquals(None))
        configuration = updates.select_single(objectName='configuration')
        self.assertThat(configuration, NotEquals(None))
        self.system_settings.main_view.scroll_to_and_click(configuration)

    def test_check_for_updates_area(self):
        """Check that the updates area is shown on opening."""
        updates = self.system_settings.main_view.updates_page
        self.assertThat(updates, NotEquals(None))
        checkForUpdatesArea = updates.select_single(
            objectName='checkForUpdatesArea')
        self.assertThat(checkForUpdatesArea, NotEquals(None))
        self.assertThat(checkForUpdatesArea.visible, Equals(True))
        self.assertThat(checkForUpdatesArea.visible,
                        Eventually(NotEquals(True)))

    def test_searching_state(self):
        """Check how the ui reacts to searching state."""
        updates = self.system_settings.main_view.updates_page
        self.assertThat(updates, NotEquals(None))
        updates.state.wait_for("SEARCHING")
        self.assertThat(updates.state, Equals("SEARCHING"))
        notification = updates.select_single(
            objectName='notification')
        self.assertThat(notification, NotEquals(None))
        self.assertThat(notification.visible, Equals(False))
        installAllButton = updates.select_single(
            objectName='installAllButton')
        self.assertThat(installAllButton, NotEquals(None))
        self.assertThat(installAllButton.visible, Equals(False))
        updateNotification = updates.select_single(
            objectName='updateNotification')
        self.assertThat(updateNotification, NotEquals(None))
        self.assertThat(updateNotification.visible, Equals(False))
        checkForUpdatesArea = updates.select_single(
            objectName='checkForUpdatesArea')
        self.assertThat(checkForUpdatesArea, NotEquals(None))
        self.assertThat(checkForUpdatesArea.visible, Equals(True))

    def test_no_updates_state(self):
        """Check how the ui reacts to no updates state."""
        updates = self.system_settings.main_view.updates_page
        self.assertThat(updates, NotEquals(None))
        updates.state.wait_for("NOUPDATES")
        self.assertThat(updates.state, Equals("NOUPDATES"))
        updateList = updates.select_single(
            objectName='updateList')
        self.assertThat(updateList, NotEquals(None))
        self.assertThat(updateList.visible, Equals(False))
        installAllButton = updates.select_single(
            objectName='installAllButton')
        self.assertThat(installAllButton, NotEquals(None))
        self.assertThat(installAllButton.visible, Equals(False))
        updateNotification = updates.select_single(
            objectName='updateNotification')
        self.assertThat(updateNotification, NotEquals(None))
        self.assertThat(updateNotification.visible, Equals(True))
