# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2013 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

from __future__ import absolute_import

from fixtures import EnvironmentVariable
from testtools.matchers import NotEquals
from ubuntu_system_settings.tests import SystemUpdatesBaseTestCase


class SystemUpdatesTestCases(SystemUpdatesBaseTestCase):

    """Tests for System Updates."""

    click_server_parameters = {
        'start': True
    }

    def setUp(self):
        # Set environment variables
        self.useFixture(
            EnvironmentVariable("IGNORE_CREDENTIALS", "True"))
        self.useFixture(
            EnvironmentVariable("URL_APPS", "http://0.0.0.0:9009/metadata"))
        super(SystemUpdatesTestCases, self).setUp()

    def test_check_for_updates_area(self):
        """Check that the updates area is shown on opening."""
        updates = self.system_settings.main_view.updates_page
        self.assertThat(updates, NotEquals(None))
        updates.stop_check()
