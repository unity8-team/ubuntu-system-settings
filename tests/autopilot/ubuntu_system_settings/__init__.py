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

from time import sleep

from autopilot import platform
from ubuntuuitoolkit import emulators as toolkit_emulators


class SystemSettings():
    """Helper class for System Settings application"""

    APP_PATH = '/usr/bin/system-settings'
    DESKTOP_FILE = '/usr/share/applications/ubuntu-system-settings.desktop'

    def __init__(self, testobj, panel=None):
        """Constructor. Launches system settings application

        :param testobj: An AutopilotTestCase object, needed to call
        testobj.launch_test_application()

        :param panel: Launch to a specific panel. Default None.
        """
        self.testobj = testobj
        self.panel = panel
        # Launches application
        self.app = self.launch(
            self.testobj,
            self.APP_PATH,
            self.DESKTOP_FILE,
            panel=self.panel)

    def launch(self, testobj, app_path, desktop_file, panel=None):
        """Launch system settings application

        :param testobj: An AutopilotTestCase object, needed to call
        testobj.launch_test_application()

        :param panel: Launch to a specific panel. Default None.

        :returns: A proxy object that represents the application. Introspection
        data is retrievable via this object.
        """
        params = [app_path]
        if platform.model() != 'Desktop':
            params.append('--desktop_file_hint={}'.format(desktop_file))

        # Launch to a specific panel
        if panel is not None:
            params.append(panel)

        app = testobj.launch_test_application(
            *params,
            app_type='qt',
            emulator_base=toolkit_emulators.UbuntuUIToolkitEmulatorBase,
            capture_output=True)

        return app

    @property
    def main_view(self):
        """Return main view"""
        return self.app.select_single(MainWindow)


class MainWindow(toolkit_emulators.MainView):
    """An emulator class that makes it easy to interact with the UI."""

    @property
    def pointer(self):
        """ Return pointer """
        return toolkit_emulators.get_pointing_device()

    def scroll_to(self, obj):
        self.close_toolbar()
        page = self.select_single(objectName='systemSettingsPage')
        page_right = page.globalRect[0] + page.globalRect[2]
        page_bottom = page.globalRect[1] + page.globalRect[3]
        page_center_x = int(page_right / 2)
        page_center_y = int(page_bottom / 2)
        while obj.globalRect[1] + obj.height > page_bottom:
            self.pointer.drag(
                page_center_x,
                page_center_y,
                page_center_x,
                page_center_y - obj.height * 2
            )
            # avoid a flick
            sleep(0.5)

    def scroll_to_and_click(self, obj):
        self.scroll_to(obj)
        self.pointer.click_object(obj)

    @property
    def about_page(self):
        """ Return 'About' page """
        return self.select_single(objectName='aboutPage')

    @property
    def cellular_page(self):
        """ Return 'About' page """
        return self.select_single(objectName='cellularPage')

    @property
    def choose_page(self):
        """ Return 'Choose carrier' page """
        return self.select_single(objectName="chooseCarrierPage")

    @property
    def licenses_page(self):
        """ Return 'License' page """
        return self.select_single(objectName='licensesPage')

    @property
    def storage_page(self):
        """ Return 'Storage' page """
        return self.select_single(objectName='storagePage')

    @property
    def updates_page(self):
        """ Return 'System Update' page """
        return self.select_single(objectName='systemUpdatesPage')
