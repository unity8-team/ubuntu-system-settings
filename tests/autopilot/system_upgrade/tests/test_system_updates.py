# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2013 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

import time
import logging
import os

from autopilot.introspection.dbus import StateNotFoundError
from autopilot.matchers import Eventually
from testtools.matchers import Equals, raises

parentdir = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
os.sys.path.insert(0,parentdir)

from ubuntu_system_settings import emulators
from ubuntu_system_settings.tests import SystemUpdatesBaseTestCase

logging.basicConfig(level=logging.INFO)


class SystemUpdatesTestCases(SystemUpdatesBaseTestCase):

    """Tests for System Updates."""

    def setUp(self):
        #self.patch_environment('IGNORE_CREDENTIALS', 'True')
        super(SystemUpdatesTestCases, self).setUp()

    def wait_for_updates_to_download(self, timeout=120):
        while timeout > 0:
            download_state = self.app.select_single(
                'PageComponent',
                objectName='systemUpdatesPage').updatesDownloaded
            if download_state == True:
                return

            timeout = timeout - 1
            time.sleep(1)
        else:
            raise ValueError(
                "Download didn't complete in given time, check your "
                "internet and try again.")

    def _get_install_dialog(self):
        return self.app.wait_select_single(
            'Dialog', objectName='dialogInstall'
        )

    def wait_for_dialog(self):
        dialog = self._get_install_dialog()

        dialog.opacity.wait_for(1.0)

    def click_install_and_restart_button(self):
        button = self.app.select_single(
            'Button', objectName='btnInstallUpdate'
        )
        
        self.pointer.click_object(button)

    def _get_installing_update_screen(self):
        return self.app.wait_select_single(
            'QQuickRectangle', objectName='installingImageUpdate'
        )

    def test_image_update_from_ui_works(self):
        """Install latest system update available."""
        logging.info('Waiting for update to download.')
        self.wait_for_updates_to_download()
        logging.info('finished waiting for update')
        self.wait_for_dialog()
        self.click_install_and_restart_button()
        installing_screen = self._get_installing_update_screen()
        self.assertThat(
            installing_screen.visible, Eventually(Equals(True))
        )

    def test_state_noupdates(self):
        """Check if system is fully updated."""
        state = self.updates_page.wait_for_state('NOUPDATES')
        self.assertThat(state, Equals('NOUPDATES'))
