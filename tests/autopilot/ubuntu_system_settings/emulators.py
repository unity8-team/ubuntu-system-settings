# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
#
# Copyright (C) 2014 Canonical Ltd.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation; version 3.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.

"""Emulators for updates page used by upgrade tests."""

import time
import logging

logger = logging.getLogger(__name__)


class UpdatesPage(object):

    def __init__(self, app):
        self.app = app

    def _get_updates_view(self):
        return self.app.select_single(
            'PageComponent', objectName='systemUpdatesPage'
        )

    def _get_install_dialog(self):
        return self.app.wait_select_single(
            'Dialog', objectName='dialogInstall'
        )

    def _wait_for_dialog(self):
        dialog = self._get_install_dialog()

        dialog.opacity.wait_for(1.0)

    def get_installing_update_screen(self):
        return self.app.wait_select_single(
            'QQuickRectangle', objectName='installingImageUpdate'
        )

    def wait_for_updates_to_download(self, timeout=40):
        """Wait for system updates to download

        :returns: None
        :raises: ValueError
        """
        while timeout > 0:
            download_state = self._get_updates_view().updatesDownloaded
            if download_state:
                return

            timeout = timeout - 1
            time.sleep(5)
        else:
            raise ValueError(
                "Download didn't complete in given time, check your "
                "internet and try again."
            )

    def wait_for_state(self, state, timeout=60):
        """Wait for expected state

        :returns: state
        :raises: SystemSettingsEmulatorException
        """
        for wait in range(timeout):
            status = self._get_updates_view().state
            logger.info(
                'State: {} waiting for {}'.format(status, state)
            )
            if state == status:
                return state
            time.sleep(1)

        raise ValueError(
            'Error state {} not found before timeout'.format(state)
        )

    def click_install_and_restart_button(self):
        """Wait for the install dialog to appear before clicking
        on its button.
        """
        self._wait_for_dialog()
        button = self.app.select_single(
            'Button', objectName='btnInstallUpdate'
        )

        self.pointer.click_object(button)
