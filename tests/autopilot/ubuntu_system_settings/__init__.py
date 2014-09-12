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

import logging

# TODO This is a workaround for bug #1327325 that will make phabet-test-run
# fail if something is printed to stdout.
logging.basicConfig(filename='warning.log', level=logging.WARNING)


from time import sleep

from autopilot.input import Keyboard
import autopilot.logging
import ubuntuuitoolkit
from autopilot import introspection, platform
from ubuntu_system_settings.utils.i18n import ugettext as _

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
            emulator_base=ubuntuuitoolkit.UbuntuUIToolkitCustomProxyObjectBase,
            capture_output=True)

        return app

    @property
    def main_view(self):
        """Return main view"""
        return self.app.select_single(MainWindow)


class MainWindow(ubuntuuitoolkit.MainView):
    """An emulator class that makes it easy to interact with the UI."""

    @autopilot.logging.log_action(logger.debug)
    def click_item(self, object_name):
        """Click a system settings item.

        :param object_name: The objectName property of the item to click.

        """
        item = self.select_single(objectName=object_name)
        item.swipe_into_view()
        self.pointing_device.click_object(item)

    @autopilot.logging.log_action(logger.debug)
    def go_to_phone_page(self):
        return self._go_to_page('entryComponent-phone', 'phonePage')

    @autopilot.logging.log_action(logger.debug)
    def go_to_reset_phone(self):
        return self._go_to_page('entryComponent-reset', 'resetPage')

    @autopilot.logging.log_action(logger.debug)
    def go_to_language_page(self):
        return self._go_to_page('entryComponent-language', 'languagePage')

    @autopilot.logging.log_action(logger.debug)
    def go_to_wifi_page(self):
        return self._go_to_page('entryComponent-wifi', 'wifiPage')

    def _go_to_page(self, item_object_name, page_object_name):
        self.click_item(item_object_name)
        page = self.wait_select_single(objectName=page_object_name)
        page.active.wait_for(True)
        return page

    def scroll_to(self, obj):
        page = self.system_settings_page
        page_right = page.globalRect[0] + page.globalRect[2]
        page_bottom = page.globalRect[1] + page.globalRect[3]
        page_center_x = int(page_right / 2)
        page_center_y = int(page_bottom / 2)
        while obj.globalRect[1] + obj.height > page_bottom:
            self.pointing_device.drag(
                page_center_x,
                page_center_y,
                page_center_x,
                page_center_y - obj.height * 2
            )
            # avoid a flick
            sleep(0.5)

    def scroll_to_and_click(self, obj):
        self.scroll_to(obj)
        self.pointing_device.click_object(obj)

    @property
    def system_settings_page(self):
        return self.select_single(objectName='systemSettingsPage')

    @property
    def cellular_page(self):
        """ Return 'Cellular' page """
        return self.select_single(objectName='cellularPage')

    @property
    def choose_page(self):
        """ Return 'Choose carrier' page """
        return self.select_single(objectName="chooseCarrierPage")

    @property
    def storage_page(self):
        """ Return 'Storage' page """
        return self.select_single(objectName='storagePage')

    @property
    def updates_page(self):
        """ Return 'System Update' page """
        return self.select_single(objectName='systemUpdatesPage')

    @property
    def background_page(self):
        """ Return 'Background' page """
        return self.select_single(objectName='backgroundPage')

    @property
    def sound_page(self):
        """ Return 'Sound' page """
        return self.select_single(objectName='soundPage')

    @property
    def security_page(self):
        """ Return 'Security' page """
        return self.select_single(objectName='securityPrivacyPage')

    @property
    def about_page(self):
        """ Return 'About' page """
        return self.select_single(objectName='aboutPage')

    @property
    def wifi_page(self):
        """ Return 'Wifi' page """
        return self.select_single(objectName='wifiPage')

    @property
    def _orientation_lock_switch(self):
        return self.wait_select_single(
            ubuntuuitoolkit.CheckBox,
            objectName='orientationLockSwitch')

    def enable_orientation_lock(self):
        self._orientation_lock_switch.check()

    def disable_orientation_lock(self):
        self._orientation_lock_switch.uncheck()


class CelullarPage(ubuntuuitoolkit.UbuntuUIToolkitCustomProxyObjectBase):

    """Autopilot helper for the Sound page."""

    @classmethod
    def validate_dbus_object(cls, path, state):
        name = introspection.get_classname_from_path(path)
        if name == b'PageComponent':
            if state['objectName'][1] == 'cellularPage':
                return True
        return False


class TimeAndDatePage(ubuntuuitoolkit.UbuntuUIToolkitCustomProxyObjectBase):

    """Autopilot helper for the Sound page."""

    @classmethod
    def validate_dbus_object(cls, path, state):
        name = introspection.get_classname_from_path(path)
        if name == b'PageComponent':
            if state['objectName'][1] == 'timeDatePage':
                return True
        return False


class SoundPage(ubuntuuitoolkit.UbuntuUIToolkitCustomProxyObjectBase):

    """Autopilot helper for the Sound page."""

    @classmethod
    def validate_dbus_object(cls, path, state):
        name = introspection.get_classname_from_path(path)
        if name == b'ItemPage':
            if state['objectName'][1] == 'soundPage':
                return True
        return False


class AboutPage(ubuntuuitoolkit.UbuntuUIToolkitCustomProxyObjectBase):

    """Autopilot helper for the About page."""

    @classmethod
    def validate_dbus_object(cls, path, state):
        name = introspection.get_classname_from_path(path)
        if name == b'ItemPage':
            if state['objectName'][1] == 'aboutPage':
                return True
        return False

    def get_device_name(self):
        device_label = self.select_single(objectName='deviceLabel')
        return device_label.text

    def is_serial_visible(self):
        serial_item = self._get_serial_item()
        return serial_item.visible

    def _get_serial_item(self):
        return self.select_single(objectName='serialItem')

    def get_serial(self):
        serial_item = self._get_serial_item()
        return serial_item.value

    def is_imei_visible(self):
        imei_item = self._get_imei_item()
        return imei_item.visible

    def _get_imei_item(self):
        return self.wait_select_single(objectName='imeiItem')

    def get_imei(self):
        imei_item = self._get_imei_item()
        return imei_item.value

    def get_os_information(self):
        os_item = self.select_single(objectName='osItem')
        return os_item.value

    def get_last_updated_date(self):
        last_updated_item = self.select_single(objectName='lastUpdatedItem')
        return last_updated_item.value

    @autopilot.logging.log_action(logger.info)
    def go_to_check_for_updates(self):
        check_for_updates_button = self.select_single(
            objectName='updateButton')
        self.pointing_device.click_object(check_for_updates_button)
        system_updates_page = self.get_root_instance().wait_select_single(
            objectName='systemUpdatesPage')
        system_updates_page.active.wait_for(True)
        return system_updates_page

    @autopilot.logging.log_action(logger.info)
    def go_to_software_licenses(self):
        license_item = self.select_single(
            ubuntuuitoolkit.listitems.Standard, objectName='licenseItem')
        license_item.swipe_into_view()
        self.pointing_device.click_object(license_item)
        licenses_page = self.get_root_instance().wait_select_single(
            objectName='licensesPage')
        licenses_page.active.wait_for(True)
        return licenses_page


class LicensesPage(ubuntuuitoolkit.UbuntuUIToolkitCustomProxyObjectBase):

    """Autopilot helper for the Licenses page."""

    @classmethod
    def validate_dbus_object(cls, path, state):
        name = introspection.get_classname_from_path(path)
        if name == b'ItemPage':
            if state['objectName'][1] == 'licensesPage':
                return True
        return False


class SystemUpdatesPage(ubuntuuitoolkit.UbuntuUIToolkitCustomProxyObjectBase):

    """Autopilot helper for the System Updates page."""

    @classmethod
    def validate_dbus_object(cls, path, state):
        name = introspection.get_classname_from_path(path)
        if name == b'PageComponent':
            if state['objectName'][1] == 'systemUpdatesPage':
                return True
        return False


class PhonePage(ubuntuuitoolkit.UbuntuUIToolkitCustomProxyObjectBase):

    """Autopilot helper for the Phone page."""

    @classmethod
    def validate_dbus_object(cls, path, state):
        name = introspection.get_classname_from_path(path)
        if name == b'PageComponent':
            if state['objectName'][1] == 'phonePage':
                return True
        return False

    @autopilot.logging.log_action(logger.info)
    def go_to_call_forwarding(self, sim=None):
        """Open the Call Forwarding settings page.

        :param sim: Number of what SIM to use, either 1 or 2.
            Required parameter in dual SIM setups
        :returns: The Call Forwarding settings page.

        """
        find = "callFwd"
        if sim:
            find = "callFwdSim%d" % sim

        return self._go_to_page(find, 'callForwardingPage')

    def _go_to_page(self, item_object_name, page_object_name):
        self._click_item(item_object_name)
        page = self.get_root_instance().wait_select_single(
            objectName=page_object_name)
        page.active.wait_for(True)
        return page

    def _click_item(self, object_name):
        item = self.select_single(objectName=object_name)
        item.swipe_into_view()
        self.pointing_device.click_object(item)

    @autopilot.logging.log_action(logger.info)
    def go_to_call_waiting(self, sim=None):
        """Open the Call Waiting settings page.

        :param sim: Number of what SIM to use, either 1 or 2.
            Required parameter in dual SIM setups
        :returns: The Call Waiting settings page.

        """
        find = "callWait"
        if sim:
            find = "callWaitSim%d" % sim
        return self._go_to_page(find, 'callWaitingPage')

    @autopilot.logging.log_action(logger.info)
    def go_to_sim_services(self, sim=None):
        """Open the SIM Services settings page.

        :param sim: Number of what SIM to use, either 1 or 2.
            Required parameter in dual SIM setups
        :returns: The SIM Services settings page.

        """
        find = "simServices"
        if sim:
            find = "simServicesSim%d" % sim

        return self._go_to_page(find, 'servicesPage')


class CallWaiting(ubuntuuitoolkit.UbuntuUIToolkitCustomProxyObjectBase):

    """Autopilot helper for the Call waiting page."""

    @property
    def _switch(self):
        return self.wait_select_single(
            ubuntuuitoolkit.CheckBox,
            objectName='callWaitingSwitch')

    def enable_call_waiting(self):
        self._switch.check()

    def disable_call_waiting(self):
        self._switch.uncheck()


class CallForwarding(ubuntuuitoolkit.UbuntuUIToolkitCustomProxyObjectBase):

    """Autopilot helper for the Call forwarding page."""

    @property
    def _switch(self):
        return self.wait_select_single(
            ubuntuuitoolkit.CheckBox,
            objectName='callForwardingSwitch')

    @property
    def _number_field(self):
        return self.wait_select_single(
            objectName='destNumberField')

    def _click_set(self):
        button = self.wait_select_single(
            objectName='set')
        self.pointing_device.click_object(button)

    def _click_cancel(self):
        button = self.wait_select_single(
            objectName='cancel')
        self.pointing_device.click_object(button)

    @property
    def current_forwarding(self):
        return self.wait_select_single(
            objectName='destNumberField').text

    def enable_call_forwarding(self):
        self._switch.check()

    def disable_call_forwarding(self):
        self._switch.uncheck()

    def set_forward(self, number):
        input_method = Keyboard.create()
        self.enable_call_forwarding()
        self.pointing_device.click_object(self._number_field)
        input_method.type(number)
        self._click_set()


class Services(ubuntuuitoolkit.UbuntuUIToolkitCustomProxyObjectBase):

    """Autopilot helper for the Call waiting page."""

    # TODO: add pages for each relevant sim services page
    def open_sim_service(self, service):
        """Return a sim service page"""
        pass


class ResetPage(ubuntuuitoolkit.UbuntuUIToolkitCustomProxyObjectBase):

    """Autopilot helper for the Reset page."""

    @classmethod
    def validate_dbus_object(cls, path, state):
        name = introspection.get_classname_from_path(path)
        if name == b'PageComponent':
            if state['objectName'][1] == 'resetPage':
                return True
        return False

    @autopilot.logging.log_action(logger.info)
    def reset_launcher(self):
        """Reset the launcher.

        :returns: The main system settings page object, that will be visible
            after the reset is complete.

        """
        confirm_dialog = self._click_reset_launcher()
        confirm_dialog.confirm_reset()
        return self._wait_and_return_main_system_settins_page()

    @autopilot.logging.log_action(logger.debug)
    def _click_reset_launcher(self):
        button = self.select_single(objectName='resetLauncher')
        self.pointing_device.click_object(button)
        return self.get_root_instance().select_single(
            objectName='resetLauncherDialog')

    def _wait_and_return_main_system_settins_page(self):
        main_view = self.get_root_instance().select_single(MainWindow)
        main_view.system_settings_page.active.wait_for(True)
        return main_view.system_settings_page

    @autopilot.logging.log_action(logger.info)
    def erase_and_reset_everything(self):
        """Reset to factory settings.

        :returns: The main system settings page object, that will be visible
            after the reset is complete.

        """
        confirm_dialog = self._click_factory_reset()
        confirm_dialog.confirm_reset()
        return self._wait_and_return_main_system_settins_page()

    @autopilot.logging.log_action(logger.debug)
    def _click_factory_reset(self):
        button = self.select_single(objectName='factoryReset')
        self.pointing_device.click_object(button)
        return self.get_root_instance().select_single(
            objectName='factoryResetDialog')


class ResetLauncherConfirmationDialog(
        ubuntuuitoolkit.UbuntuUIToolkitCustomProxyObjectBase):

    """Autopilot helper for the Reset Launcher Confirmation dialog."""

    @classmethod
    def validate_dbus_object(cls, path, state):
        name = introspection.get_classname_from_path(path)
        if name == b'Dialog':
            if state['objectName'][1] == 'resetLauncherDialog':
                return True
        return False

    @autopilot.logging.log_action(logger.debug)
    def confirm_reset(self):
        button = self.select_single('Button', objectName='resetLauncherAction')
        self.pointing_device.click_object(button)


class FactoryResetConfirmationDialog(
        ubuntuuitoolkit.UbuntuUIToolkitCustomProxyObjectBase):

    """Autopilot helper for the Reset Launcher Confirmation dialog."""

    @classmethod
    def validate_dbus_object(cls, path, state):
        name = introspection.get_classname_from_path(path)
        if name == b'Dialog':
            if state['objectName'][1] == 'factoryResetDialog':
                return True
        return False

    @autopilot.logging.log_action(logger.debug)
    def confirm_reset(self):
        button = self.select_single('Button', objectName='factoryResetAction')
        self.pointing_device.click_object(button)


class LanguagePage(ubuntuuitoolkit.UbuntuUIToolkitCustomProxyObjectBase):

    """Autopilot helper for the Language page."""

    @classmethod
    def validate_dbus_object(cls, path, state):
        name = introspection.get_classname_from_path(path)
        if name == b'ItemPage':
            if state['objectName'][1] == 'languagePage':
                return True
        return False

    def get_current_language(self):
        return self.select_single(
            'Label', objectName='currentLanguage').currentLanguage

    def _click_change_display_language(self):
        item = self.select_single(objectName='displayLanguage')
        self.pointing_device.click_object(item)
        return self.get_root_instance().select_single(
            objectName='displayLanguageDialog')

    @autopilot.logging.log_action(logger.info)
    def change_display_language(self, langIndex, reboot=True):
        """Changes display language.

        :param langIndex: The language index to change to.

        :param reboot: Whether to reboot or not

        :returns: The language page

        """
        dialog = self._click_change_display_language()
        dialog.set_language(langIndex, reboot)
        return self.get_root_instance()


class DisplayLanguage(ubuntuuitoolkit.UbuntuUIToolkitCustomProxyObjectBase):

    """Autopilot helper for the Display Language dialog."""

    @autopilot.logging.log_action(logger.debug)
    def set_language(self, index, reboot):
        self._click_language_item(index)
        reboot_dialog = self._click_confirm()

        if reboot:
            reboot_dialog.reboot()
        else:
            reboot_dialog.revert()

    @autopilot.logging.log_action(logger.debug)
    def _click_language_item(self, index):
        languages_list = self.select_single('QQuickListView',
                                            objectName='languagesList')
        languages_list.click_element('languageName%d' % index)

    @autopilot.logging.log_action(logger.debug)
    def _click_confirm(self):
        button = self.select_single('Button',
                                    objectName='confirmChangeLanguage')
        self.pointing_device.click_object(button)
        return self.get_root_instance().select_single(
            objectName='rebootNecessaryDialog')

    @autopilot.logging.log_action(logger.debug)
    def _click_cancel(self):
        button = self.select_single('Button',
                                    objectName='cancelChangeLanguage')
        self.pointing_device.click_object(button)


class RebootNecessary(
        ubuntuuitoolkit.UbuntuUIToolkitCustomProxyObjectBase):

    """Autopilot helper for the Reboot Necessary dialog."""

    @autopilot.logging.log_action(logger.debug)
    def reboot(self):
        self._click_reboot()

    @autopilot.logging.log_action(logger.debug)
    def revert(self):
        self._click_revert()

    @autopilot.logging.log_action(logger.debug)
    def _click_reboot(self):
        button = self.select_single('Button', objectName='reboot')
        self.pointing_device.click_object(button)

    @autopilot.logging.log_action(logger.debug)
    def _click_revert(self):
        button = self.select_single('Button', objectName='revert')
        self.pointing_device.click_object(button)


class WifiPage(ubuntuuitoolkit.UbuntuUIToolkitCustomProxyObjectBase):

    """Autopilot helper for the Sound page."""

    @classmethod
    def validate_dbus_object(cls, path, state):
        name = introspection.get_classname_from_path(path)
        if name == b'ItemPage':
            if state['objectName'][1] == 'wifiPage':
                return True
        return False

    @autopilot.logging.log_action(logger.debug)
    def connect_to_hidden_network(self, name, security="none", password=None,
                                  cancel=False):
        dialog = self._click_connect_to_hidden_network()
        dialog.enter_name(name)
        if security:
            dialog.set_security(security)
        if password:
            dialog.enter_password(password)

        if cancel:
            dialog.cancel()
        else:
            dialog.connect()

    @autopilot.logging.log_action(logger.debug)
    def _click_connect_to_hidden_network(self):

        # we can't mock the qunitymenu items, so we
        # have to wait for them to be built
        sleep(1)

        button = self.select_single('*',
                                    objectName='connectToHiddenNetwork')
        self.pointing_device.click_object(button)
        return self.get_root_instance().wait_select_single(
            objectName='otherNetworkDialog')


class OtherNetwork(
        ubuntuuitoolkit.UbuntuUIToolkitCustomProxyObjectBase):

    """Autopilot helper for the Connect to Hidden Network dialog."""

    @classmethod
    def validate_dbus_object(cls, path, state):
        name = introspection.get_classname_from_path(path)
        if name == b'Dialog':
            if state['objectName'][1] == 'otherNetworkDialog':
                return True
        return False

    @autopilot.logging.log_action(logger.debug)
    def enter_name(self, name):
        self._enter_name(name)

    @autopilot.logging.log_action(logger.debug)
    def _enter_name(self, name):
        namefield = self.select_single('TextField',
                                       objectName='networkname')
        namefield.write(name)

    @autopilot.logging.log_action(logger.debug)
    def set_security(self, security):
        """Sets the hidden network's security

        :param security: Either "none", "wpa" or "wep

        :returns: None

        """
        self._set_security(security)

    @autopilot.logging.log_action(logger.debug)
    def _expand_security_list(self):
        sec_list = self.select_single(
            '*', objectName='securityList')
        active_child = sec_list.select_single(
            '*', objectName='listContainer')
        self.pointing_device.click_object(active_child)
        # wait for it to expand
        sleep(0.5)
        return sec_list

    @autopilot.logging.log_action(logger.debug)
    def _set_security(self, security):
        if security == 'none':
            sec_list = self._expand_security_list()
            item = sec_list.wait_select_single('*',
                                               text=_('None'))
            self.pointing_device.click_object(item)
        elif security == 'wpa':
            sec_list = self._expand_security_list()
            item = sec_list.wait_select_single('*',
                                               text=_('WPA & WPA2 Personal'))
            self.pointing_device.click_object(item)
        elif security == 'wep':
            sec_list = self._expand_security_list()
            item = sec_list.wait_select_single('*',
                                               text=_('WEP'))
            self.pointing_device.click_object(item)
        elif security is not None:
            raise ValueError('security type %s is not valid' % security)
        # wait for ui to change
        sleep(0.5)

    @autopilot.logging.log_action(logger.debug)
    def enter_password(self, password):
        self._enter_password(password)

    @autopilot.logging.log_action(logger.debug)
    def _enter_password(self, password):
        pwdfield = self.select_single('TextField',
                                      objectName='password')
        pwdfield.write(password)

    @autopilot.logging.log_action(logger.debug)
    def cancel(self):
        self._click_cancel()

    @autopilot.logging.log_action(logger.debug)
    def _click_cancel(self):
        button = self.select_single('Button', objectName='cancel')
        self.pointing_device.click_object(button)

    @autopilot.logging.log_action(logger.debug)
    def connect(self):
        self._click_connect()

    @autopilot.logging.log_action(logger.debug)
    def _click_connect(self):
        button = self.select_single('Button', objectName='connect')
        self.pointing_device.click_object(button)
