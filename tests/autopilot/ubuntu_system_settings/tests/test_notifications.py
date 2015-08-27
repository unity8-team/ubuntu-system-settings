# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2013 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

import json
import os
import subprocess

from autopilot.matchers import Eventually

from testtools.matchers import Equals

from ubuntu_system_settings.tests import UbuntuSystemSettingsTestCase

from ubuntuuitoolkit import emulators as toolkit_emulators

""" Tests for Ubuntu System Settings """


def has_helper(package):
    """Return True if  package['hooks']['foo']['push-helper'] exists"""
    return any(package['hooks'][hook].get('push-helper', None)
               for hook in package['hooks'])


class NotificationsTestCases(UbuntuSystemSettingsTestCase):
    """ Tests for Search """

    def setUp(self):
        # Check legacy items: one for each file in legacy-helpers
        super(NotificationsTestCases, self).setUp()
        self.notification_page = self.main_view.go_to_notification_page()

    def test_item_counts(self):
        """ Checks whether the Notificatins category is available """
        try:
            legacy_count = len(
                os.listdir("/usr/lib/ubuntu-push-client/legacy-helpers/"))
        except os.FileNotFoundError:
            legacy_count = 0
        # Get output of click list --manifest, and parse for the rest
        packages = json.loads(
            subprocess.check_output(
                ['click', 'list', '--manifest']).decode('utf8'))
        click_count = len([x for x in packages if has_helper(x)])

        self.assertThat(
            lambda: len(
                self.notification_page.select_many(toolkit_emulators.Standard)
            ),
            Eventually(Equals(click_count + legacy_count))
        )
