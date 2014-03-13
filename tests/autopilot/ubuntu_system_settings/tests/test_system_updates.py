# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2013 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

from autopilot.introspection.dbus import StateNotFoundError
from testtools.matchers import NotEquals, raises

from ubuntu_system_settings.tests import SystemUpdatesBaseTestCase


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

    def test_updates_not_in_main(self):
        """Check that the updates notification is shown in main."""
        self.assertThat(lambda: self.main_view.select_single(
            objectName='entryComponent-updates'), raises(StateNotFoundError))

    def test_searching_state(self):
        """Check how the ui reacts to searching state."""
        updates = self.updates_page
        self.assertThat(updates, NotEquals(None))
