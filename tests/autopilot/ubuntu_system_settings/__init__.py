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
import logging

from autopilot import platform
import autopilot.logging

from ubuntuuitoolkit import emulators as toolkit_emulators

logger = logging.getLogger(__name__)


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
            emulator_base=toolkit_emulators.UbuntuUIToolkitEmulatorBase)

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

    @autopilot.logging.log_action(logger.info)
    def go_to_sound_page(self):
        """Open the sound settings page.

        :return: sound settings page.

        """
        sound_button = self.select_single(
            'EntryComponent', objectName='entryComponent-sound'
        )
        self.scroll_to_and_click(sound_button)

        return self.wait_select_single(ItemPage, objectName='soundPage')

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

    @property
    def sound_page(self):
        """Return the 'Sound' page."""
        return self.select_single('ItemPage', objectName='soundPage')


class ItemPage(toolkit_emulators.UbuntuUIToolkitEmulatorBase):

    def _get_ringtone_setting_button(self):
        return self.wait_select_single(
            'SingleValue', objectName='ringtoneListItem'
        )

    def get_ringtone_setting_button_current_value(self):
        """current value of the ringtone setting button.

        :return: name of the currently selected ringtone.

        """
        return self._get_ringtone_setting_button().value

    @autopilot.logging.log_action(logger.info)
    def open_ringtone_selector(self):
        """Open the ringtone selector.

        :return: The page with ringtones list.

        """
        ringtone_setting_button = self._get_ringtone_setting_button()
        self.pointing_device.click_object(ringtone_setting_button)

        root = self.get_root_instance()
        return root.wait_select_single(SoundsList)


class SoundsList(ItemPage):

    @autopilot.logging.log_action(logger.info)
    def choose_ringtone(self, name):
        """Choose a new ringtone.

        :param name: name of the ringtone to select.
        :return: newly selected ringtone item.

        """
        list_view = self.select_single('QQuickListView', objectName='listView')
        # When last item of a long list is preselected the list scrolls
        # down automatically, wait for the list to scroll down before
        # trying to do anything. -- om26er.
        sleep(1)
        list_view.moving.wait_for(False)
        list_view.click_element('ringtone-' + name)

        return self.select_single(
            'OptionSelectorDelegate', objectName='ringtone-' + name
        )

    @autopilot.logging.log_action(logger.info)
    def go_back_to_sound_page(self):
        """Go back to the sound settings main page."""
        main_window = self.get_root_instance().select_single(MainWindow)
        main_window.open_toolbar().click_back_button()
