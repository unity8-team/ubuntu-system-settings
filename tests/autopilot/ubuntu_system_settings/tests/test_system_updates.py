# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2013 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

from time import sleep

from autopilot.matchers import Eventually
from autopilot.introspection.dbus import StateNotFoundError
from testtools.matchers import Contains, Equals, NotEquals, GreaterThan
from unittest import skip

from ubuntu_system_settings.tests import SystemUpdatesBaseTestCase
from ubuntu_system_settings.utils.i18n import ugettext as _


""" Tests for Ubuntu System Settings """


class SystemUpdatesTestCases(SystemUpdatesBaseTestCase):

    """Tests for System Updates."""

    def setUp(self):
        # Set environment variables
        super(SystemUpdatesTestCases, self).setUp()

    def test_show_updates(self):
        """ Checks whether Search box actually filters the results """
        updates = self.updates_page
        self.assertThat(updates, NotEquals(None))
        # Move to text field
        self.pointer.move_to_object(updates)
        self.pointer.click()

    def test_no_updates_in_main(self):
        """Check that the upadtes notification is not shown in main."""
        updatesNotification = self.main_view.select_single(
            objectName='updatesNotification')
        self.assertThat(updatesNotification.visible, NotEquals(True))

    def test_updates_in_main(self):
        """Check that the upadtes notification is shown in main."""
        updatesNotification = self.main_view.select_single(
            objectName='updatesNotification')
        updatesNotification.updatesAvailable = 1
        self.assertThat(updatesNotification.visible, NotEquals(False))