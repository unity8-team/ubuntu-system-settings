
# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
#
# Copyright (C) 2014, 2015 Canonical Ltd.
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

import autopilot.logging
import ubuntuuitoolkit
from autopilot import introspection
from autopilot.exceptions import StateNotFoundError
from ubuntu_system_settings.utils.i18n import ugettext as _

logger = logging.getLogger(__name__)


class SystemSettings(ubuntuuitoolkit.UbuntuUIToolkitCustomProxyObjectBase):

    """Helper class for System Settings application"""

    @classmethod
    def validate_dbus_object(cls, path, state):
        name = introspection.get_classname_from_path(path)
        return ((name == b'SystemSettings' and
                 state['applicationName'][1] == 'SystemSettings') or
                (name == b'ubuntu-system-settings' and
                 state['applicationName'][1] == 'ubuntu-system-settings'))

    @property
    def main_view(self):
        """Return main view"""
        return self.select_single(objectName='systemSettingsMainView')


class SystemSettingsMainWindow(ubuntuuitoolkit.MainView):

    """Autopilot helper for the Main Window."""

    @classmethod
    def validate_dbus_object(cls, path, state):
        name = introspection.get_classname_from_path(path)
        return (name == b'MainWindow' and
                state['objectName'][1] == 'systemSettingsMainView')

    @autopilot.logging.log_action(logger.debug)
    def click_item(self, object_name):
        """Click a system settings item.

        :param object_name: The objectName property of the item to click.

        """
        item = self.select_single(objectName=object_name)
        item.swipe_into_view()
        self.pointing_device.click_object(item)

    @autopilot.logging.log_action(logger.debug)
    def go_to_reset_phone(self):
        return self._go_to_page('entryComponent-reset', 'resetPage')

    @autopilot.logging.log_action(logger.debug)
    def go_to_language_page(self):
        return self._go_to_page('entryComponent-language', 'languagePage')

    @autopilot.logging.log_action(logger.debug)
    def go_to_wifi_page(self):
        return self._go_to_page('entryComponent-wifi', 'wifiPage')

    @autopilot.logging.log_action(logger.debug)
    def go_to_cellular_page(self):
        return self._go_to_page('entryComponent-cellular', 'cellularPage')

    @autopilot.logging.log_action(logger.debug)
    def go_to_bluetooth_page(self):
        return self._go_to_page('entryComponent-bluetooth', 'bluetoothPage')

    @autopilot.logging.log_action(logger.debug)
    def go_to_phone_page(self):
        return self._go_to_page('entryComponent-phone', 'phonePage')

    @autopilot.logging.log_action(logger.debug)
    def go_to_about_page(self):
        return self._go_to_page('entryComponent-about', 'aboutPage')

    @autopilot.logging.log_action(logger.debug)
    def go_to_sound_page(self):
        return self._go_to_page('entryComponent-sound', 'soundPage')

    @autopilot.logging.log_action(logger.debug)
    def go_to_security_page(self):
        return self._go_to_page('entryComponent-security-privacy',
                                'securityPrivacyPage')

    @autopilot.logging.log_action(logger.debug)
    def go_to_datetime_page(self):
        return self._go_to_page('entryComponent-time-date', 'timeDatePage')

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
    def _orientation_lock_switch(self):
        return self.wait_select_single(
            ubuntuuitoolkit.CheckBox,
            objectName='orientationLockSwitch')

    def enable_orientation_lock(self):
        self._orientation_lock_switch.check()

    def disable_orientation_lock(self):
        self._orientation_lock_switch.uncheck()


class Dialog(ubuntuuitoolkit.Dialog):
    # XXX A new Dialog custom proxy object was added to the toolkit.
    # Because of https://bugs.launchpad.net/autopilot-qt/+bug/1341671
    # we need to make sure it does not match in any selection.

    @classmethod
    def validate_dbus_object(cls, path, state):
        return False


class CellularPage(ubuntuuitoolkit.UbuntuUIToolkitCustomProxyObjectBase):

    """Autopilot helper for the Cellular page."""

    @classmethod
    def validate_dbus_object(cls, path, state):
        name = introspection.get_classname_from_path(path)
        if name == b'PageComponent':
            if state['objectName'][1] == 'cellularPage':
                return True
        return False

    @autopilot.logging.log_action(logger.debug)
    def enable_data(self):
        self._set_data(True)

    @autopilot.logging.log_action(logger.debug)
    def disable_data(self):
        self._set_data(False)

    def disable_datas(self):
        self.select_sim_for_data('off')

    @autopilot.logging.log_action(logger.debug)
    def _set_data(self, data):
        chk = self.select_single(objectName='data')
        if data:
            chk.check()
        else:
            chk.uncheck()

    @autopilot.logging.log_action(logger.debug)
    def get_data(self):
        return self.select_single(objectName='data').checked

    @autopilot.logging.log_action(logger.debug)
    def enable_roaming(self, timeout=10):
        self._set_roaming(True, timeout=timeout)

    @autopilot.logging.log_action(logger.debug)
    def disable_roaming(self, timeout=10):
        self._set_roaming(False, timeout=timeout)

    @autopilot.logging.log_action(logger.debug)
    def _set_roaming(self, roaming, timeout):
        chk = self.select_single(objectName='roaming')
        if roaming:
            chk.check(timeout=timeout)
        else:
            chk.uncheck(timeout=timeout)

    @autopilot.logging.log_action(logger.debug)
    def set_connection_type(self, radio_type, sim='/ril_0',
                            scroll_to_and_click=None):
        self._set_connection_type(radio_type, sim, scroll_to_and_click)

    @autopilot.logging.log_action(logger.debug)
    def _set_connection_type(self, radio_type, sim, scroll_to_and_click):
        t = self.wait_select_single(
            'OptionSelectorDelegate',
            objectName='%s_radio_%s' % (sim, radio_type))
        if scroll_to_and_click:
            scroll_to_and_click(t)
        else:
            t.swipe_into_view()

        self.pointing_device.click_object(t)

    @autopilot.logging.log_action(logger.debug)
    def change_carrier(self, carrier, sim=None):
        carrierApnPage = self._click_carrier_apn()
        chooseCarrierPage = carrierApnPage.open_carrier(sim)
        chooseCarrierPage.set_carrier(carrier)

    @autopilot.logging.log_action(logger.debug)
    def _click_carrier_apn(self):
        item = self.select_single(objectName='carrierApnEntry')
        self.pointing_device.click_object(item)
        return self.get_root_instance().wait_select_single(
            objectName='carrierApnPage')

    @autopilot.logging.log_action(logger.debug)
    def select_sim_for_data(self, sim):
        self._select_sim_for_data(sim)

    @autopilot.logging.log_action(logger.debug)
    def _select_sim_for_data(self, sim):
        item = self.select_single(objectName='use%s' % sim)
        self.pointing_device.click_object(item)

    @autopilot.logging.log_action(logger.debug)
    def select_sim_for_calls(self, sim):
        pass

    @autopilot.logging.log_action(logger.debug)
    def select_sim_for_messages(self):
        pass

    @autopilot.logging.log_action(logger.debug)
    def set_name(self, sim, name):
        self._set_name(sim, name)

    def get_name(self, sim):
        obj = self.select_single(
            objectName="label_%s" % sim)
        return obj.text

    def _set_name(self, sim, name):
        obj = self.select_single(
            objectName="edit_name_%s" % sim)
        self.pointing_device.click_object(obj)

        # wait for animation
        sleep(1)
        ok = self.select_single('Button', objectName="doRename")

        field = self.select_single('TextField', objectName="nameField")
        field.write(name)
        self.pointing_device.click_object(ok)

    """
    :returns: Whether or not hotspot can be used.
    """
    @autopilot.logging.log_action(logger.debug)
    def have_hotspot(self):
        return self.wait_select_single(objectName='hotspotEntry').visible

    """
    :param: Configuration with keys ssid and password.
    :returns: Hotspot page.
    """
    @autopilot.logging.log_action(logger.debug)
    def setup_hotspot(self, config=None):
        hotspot_page = self._enter_hotspot()
        hotspot_page.setup_hotspot(config)
        return hotspot_page

    """
    Enables hotspot.
    :returns: Hotspot page.
    """
    @autopilot.logging.log_action(logger.debug)
    def enable_hotspot(self):
        hotspot_page = self._enter_hotspot()
        hotspot_page.enable_hotspot()
        return hotspot_page

    """
    Disables hotspot.
    :returns: Hotspot page.
    """
    @autopilot.logging.log_action(logger.debug)
    def disable_hotspot(self):
        hotspot_page = self._enter_hotspot()
        hotspot_page.disable_hotspot()
        return hotspot_page

    @autopilot.logging.log_action(logger.debug)
    def _enter_hotspot(self):
        obj = self.wait_select_single(objectName="hotspotEntry")
        self.pointing_device.click_object(obj)
        return self.get_root_instance().wait_select_single(
            objectName='hotspotPage')


class Hotspot(ubuntuuitoolkit.UbuntuUIToolkitCustomProxyObjectBase):

    """Autopilot helper for Hotspot page."""

    @classmethod
    def validate_dbus_object(cls, path, state):
        name = introspection.get_classname_from_path(path)
        if name == b'ItemPage':
            if state['objectName'][1] == 'hotspotPage':
                return True
        return False

    @property
    def _switch(self):
        return self.wait_select_single(
            ubuntuuitoolkit.CheckBox,
            objectName='hotspotSwitch')

    @autopilot.logging.log_action(logger.debug)
    def enable_hotspot(self):
        self._switch.check()

    @autopilot.logging.log_action(logger.debug)
    def disable_hotspot(self):
        self._switch.uncheck()

    @autopilot.logging.log_action(logger.debug)
    def setup_hotspot(self, config):
        obj = self.select_single(objectName='hotspotSetupEntry')
        self.pointing_device.click_object(obj)
        setup = self.get_root_instance().wait_select_single(
            objectName='hotspotSetup')
        if config:
            if 'ssid' in config:
                setup.set_ssid(config['ssid'])
            if 'password' in config:
                setup.set_password(config['password'])
        setup.enable()
        if setup:
            setup.wait_until_destroyed()

    @autopilot.logging.log_action(logger.debug)
    def get_hotspot_status(self):
        return self._switch.checked


class HotspotSetup(ubuntuuitoolkit.UbuntuUIToolkitCustomProxyObjectBase):

    """Autopilot helper for Hotspot setup."""

    @classmethod
    def validate_dbus_object(cls, path, state):
        name = introspection.get_classname_from_path(path)
        if name == b'Dialog':
            if state['objectName'][1] == 'hotspotSetup':
                return True
        return False

    @property
    def _ssid_field(self):
        return self.wait_select_single(
            ubuntuuitoolkit.TextField,
            objectName='ssidField')

    @property
    def _password_field(self):
        return self.wait_select_single(
            ubuntuuitoolkit.TextField,
            objectName='passwordField')

    @property
    def _enable_button(self):
        return self.wait_select_single(
            'Button', objectName='confirmButton')

    @autopilot.logging.log_action(logger.debug)
    def set_ssid(self, ssid):
        self._ssid_field.write(ssid)

    @autopilot.logging.log_action(logger.debug)
    def set_password(self, password):
        self._password_field.write(password)

    @autopilot.logging.log_action(logger.debug)
    def enable(self):
        self.pointing_device.click_object(self._enable_button)


class BluetoothPage(ubuntuuitoolkit.UbuntuUIToolkitCustomProxyObjectBase):

    """Autopilot helper for Bluetooth page."""

    @classmethod
    def validate_dbus_object(cls, path, state):
        name = introspection.get_classname_from_path(path)
        if name == b'PageComponent':
            if state['objectName'][1] == 'bluetoothPage':
                return True
        return False

    def get_disconnected_devices(self):
        """Return the list of known disconnected devices.

        :return: a list containing the text for each device

        """
        disconnected_list = self.select_single(
            'QQuickColumn',
            objectName='disconnectedList'
        )
        # NOTE: the UI design uses ellipsis to be suggestive
        ellipsis = '\u2026'
        return [device.text.strip(ellipsis) for device in
                disconnected_list.select_many('LabelVisual')]


class PageCarrierAndApn(ubuntuuitoolkit.UbuntuUIToolkitCustomProxyObjectBase):

    """Autopilot helper for carrier/apn entry page (singlesim)."""
    @autopilot.logging.log_action(logger.debug)
    def open_carrier(self, sim):
        return self._click_carrier(sim)

    @autopilot.logging.log_action(logger.debug)
    def _click_carrier(self, sim):
        obj = self.select_single(
            objectName='carrier')
        self.pointing_device.click_object(obj)
        return self.get_root_instance().wait_select_single(
            objectName='chooseCarrierPage')


class PageCarriersAndApns(
        ubuntuuitoolkit.UbuntuUIToolkitCustomProxyObjectBase):
    """Autopilot helper for carrier/apn entry page (multisim)."""
    """Autopilot helper for carrier/apn entry page (singlesim)."""
    @autopilot.logging.log_action(logger.debug)
    def open_carrier(self, sim):
        return self._click_carrier(sim)

    @autopilot.logging.log_action(logger.debug)
    def _click_carrier(self, sim):
        obj = self.select_single(
            objectName='%s_carriers' % sim)
        self.pointing_device.click_object(obj)
        return self.get_root_instance().wait_select_single(
            objectName='chooseCarrierPage')


class PageChooseCarrier(ubuntuuitoolkit.UbuntuUIToolkitCustomProxyObjectBase):

    """Autopilot helper for carrier selection page"""

    @autopilot.logging.log_action(logger.debug)
    def set_automatic(self):
        item = self.select_single(text='Automatically')
        self.pointing_device.click_object(item)

    def set_carrier(self, carrier):
        # wait for animation, since page.animationRunning.wait_for(False)
        # does not work?
        sleep(0.5)
        allOperators = self.select_single(objectName="allOperators")
        otherOperators = self.select_single(objectName="otherOperators")

        if allOperators.visible:
            opList = allOperators
        elif otherOperators.visible:
            opList = otherOperators
        else:
            raise Exception("No operator list visible.")

        item = opList.select_single(text=carrier, objectName="carrier")
        self.pointing_device.click_object(item)


class SecurityPage(ubuntuuitoolkit.QQuickFlickable):

    """Autopilot helper for the Security page."""

    @classmethod
    def validate_dbus_object(cls, path, state):
        name = introspection.get_classname_from_path(path)
        if name == b'PageComponent':
            if state['objectName'][1] == 'securityPrivacyPage':
                return True
        return False

    @autopilot.logging.log_action(logger.debug)
    def go_to_sim_lock(self):
        selector = self.select_single(objectName='simControl')
        self.swipe_child_into_view(selector)
        self.pointing_device.click_object(selector)
        return self.get_root_instance().wait_select_single(
            objectName='simPinPage'
        )


class SimPin(ubuntuuitoolkit.QQuickFlickable):

    """Autopilot helper for the SimPin Page."""

    def get_sim_pin_switch(self, sim_number):
        """Return the SIM PIN switch."""
        switches = self.select_many(objectName='simPinSwitch')
        switches = sorted(switches, key=lambda switch: (switch.globalRect.y))
        return switches[sim_number]

    @autopilot.logging.log_action(logger.debug)
    def click_sim_pin_switch(self, sim_number):
        """Click on the SIM PIN switch, return the SIM PIN dialog."""
        sim_pin_switch = self.get_sim_pin_switch(sim_number)
        self.pointing_device.click_object(sim_pin_switch)
        return self.get_root_instance().wait_select_single(
            objectName='lockDialogComponent'
        )

    @autopilot.logging.log_action(logger.debug)
    def enter_lock_pin(self, pin):
        """Enter the given pin into our dialog."""
        root_instance = self.get_root_instance()
        prev_input = root_instance.wait_select_single(
            objectName='prevInput'
        )
        prev_input.write(pin)
        lock_unlock_button = root_instance.select_single(
            objectName='lockButton'
        )
        self.pointing_device.click_object(lock_unlock_button)


class TimeAndDatePage(ubuntuuitoolkit.UbuntuUIToolkitCustomProxyObjectBase):

    """Autopilot helper for the TimeAndDate page."""

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
        if name == b'PageComponent':
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
        check_for_updates_button.swipe_into_view()
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

    def get_number(self, obj):
        number = self.select_single(objectName=obj)
        return number.value


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
    def go_to_sim_services(self, sim=None):
        """Open the SIM Services settings page.

        :param sim: Number of what SIM to use, either 1 or 2.
            Required parameter in dual SIM setups
        :returns: The SIM Services settings page.

        """
        find = "simServices"
        if sim is not None:
            find = "simServicesSim%d" % sim

        return self._go_to_page(find, 'servicesPage')

    def get_sim_services_enabled(self, sim=None):
        """Return whether or not Sim Services is enabled.

        :param sim: Number of what SIM to use, either 1 or 2.
            Required parameter in dual SIM setups
        :returns: Whether or not Sim Services is enabled.

        """
        find = "simServices"
        if sim is not None:
            find = "simServicesSim%d" % sim
        return self.select_single(objectName=find).enabled

    @property
    def _dialpad_sounds(self):
        """The dialpad sounds switch."""
        return self.wait_select_single(
            ubuntuuitoolkit.CheckBox,
            objectName='dialpadSounds')

    @autopilot.logging.log_action(logger.info)
    def enable_dialpad_sounds(self):
        """Enable dialpad sounds."""
        self._dialpad_sounds.check()

    @autopilot.logging.log_action(logger.info)
    def disable_dialpad_sounds(self):
        """Disable dialpad sounds."""
        self._dialpad_sounds.uncheck()

    @autopilot.logging.log_action(logger.info)
    def _enter_call_waiting(self, sim=None):
        """Open the Call Waiting settings page.

        :param sim: Number of what SIM to use, either 1 or 2.
            Required parameter in dual SIM setups
        :returns: The Call Waiting settings page.

        """
        find = "callWait"
        if sim is not None:
            find = "callWaitSim%d" % sim
        return self._go_to_page(find, 'callWaitingPage')

    @autopilot.logging.log_action(logger.info)
    def enable_call_waiting(self, sim):
        self._enter_call_waiting(sim).enable_call_waiting()

    @autopilot.logging.log_action(logger.info)
    def disable_call_waiting(self, sim):
        self._enter_call_waiting(sim).disable_call_waiting()

    @autopilot.logging.log_action(logger.info)
    def _enter_call_forwarding(self, sim=None):
        """Open the Call Forwarding settings page.

        :param sim: Number of what SIM to use, either 1 or 2.
            Required parameter in dual SIM setups
        :returns: The Call Forwarding settings page.

        """
        find = "callFwd"
        if sim is not None:
            find = "callFwdSim%d" % sim

        return self._go_to_page(find, 'callForwardingPage')

    @autopilot.logging.log_action(logger.info)
    def set_forward_unconditionally(self, number, sim=None):
        """Sets forwarding unconditionally to number on a sim.

        :param sim: Number of what SIM to use, either 1 or 2.
            Required parameter in dual SIM setups
        :param number: Number to which we want to forward.
        :returns: The Call Forwarding settings page.
        """
        fwd_page = self._enter_call_forwarding(sim)
        fwd_page.set_forward_unconditionally(number)
        return fwd_page

    @autopilot.logging.log_action(logger.info)
    def unset_forward_unconditionally(self, sim=None):
        """Disables forwarding unconditionally on a sim.

        :param sim: Number of what SIM to use, either 1 or 2.
            Required parameter in dual SIM setups
        :returns: The Call Forwarding settings page.
        """
        fwd_page = self._enter_call_forwarding(sim)
        fwd_page.unset_forward_unconditionally()
        return fwd_page

    @autopilot.logging.log_action(logger.info)
    def get_forward_unconditionally(self, sim=None):
        """Return forwarding unconditionally value on sim.

        :param sim: Number of what SIM to use, either 1 or 2.
            Required parameter in dual SIM setups
        :returns: The forward unconditionally value.
        """
        fwd_page = self._enter_call_forwarding(sim)
        return fwd_page.get_forward_unconditionally()

    @autopilot.logging.log_action(logger.info)
    def set_forward_on_busy(self, number, sim=None):
        """Sets forwarding when busy to number on a sim.

        :param sim: Number of what SIM to use, either 1 or 2.
            Required parameter in dual SIM setups
        :param number: Number to which we want to forward.
        :returns: The Call Forwarding settings page.
        """
        fwd_page = self._enter_call_forwarding(sim)
        fwd_page.set_forward_on_busy(number)
        return fwd_page

    @autopilot.logging.log_action(logger.info)
    def unset_forward_on_busy(self, sim=None):
        """Disables forwarding when busy on a sim.

        :param sim: Number of what SIM to use, either 1 or 2.
            Required parameter in dual SIM setups
        :returns: The Call Forwarding settings page.
        """
        fwd_page = self._enter_call_forwarding(sim)
        fwd_page.unset_forward_on_busy()
        return fwd_page

    @autopilot.logging.log_action(logger.info)
    def get_forward_on_busy(self, sim=None):
        """Return forwarding on busy value on sim.

        :param sim: Number of what SIM to use, either 1 or 2.
            Required parameter in dual SIM setups
        :returns: The forward on busy value.
        """
        fwd_page = self._enter_call_forwarding(sim)
        return fwd_page.get_forward_on_busy()

    @autopilot.logging.log_action(logger.info)
    def set_forward_when_no_answer(self, number, sim=None):
        """Sets forwarding when no answer to number on a sim.

        :param sim: Number of what SIM to use, either 1 or 2.
            Required parameter in dual SIM setups
        :param number: Number to which we want to forward.
        :returns: The Call Forwarding settings page.
        """
        fwd_page = self._enter_call_forwarding(sim)
        fwd_page.set_forward_when_no_answer(number)
        return fwd_page

    @autopilot.logging.log_action(logger.info)
    def unset_forward_when_no_answer(self, sim=None):
        """Disables forwarding when no answer on a sim.

        :param sim: Number of what SIM to use, either 1 or 2.
            Required parameter in dual SIM setups
        :returns: The Call Forwarding settings page.
        """
        fwd_page = self._enter_call_forwarding(sim)
        fwd_page.unset_forward_when_no_answer()
        return fwd_page

    @autopilot.logging.log_action(logger.info)
    def get_forward_when_no_answer(self, sim=None):
        """Return forwarding when no answer value on sim.

        :param sim: Number of what SIM to use, either 1 or 2.
            Required parameter in dual SIM setups
        :returns: The forward when no answer value.
        """
        fwd_page = self._enter_call_forwarding(sim)
        return fwd_page.get_forward_when_no_answer()

    @autopilot.logging.log_action(logger.info)
    def set_forward_when_unreachable(self, number, sim=None):
        """Sets forwarding when unreachable to number on a sim.

        :param sim: Number of what SIM to use, either 1 or 2.
            Required parameter in dual SIM setups
        :param number: Number to which we want to forward.
        :returns: The Call Forwarding settings page.
        """
        fwd_page = self._enter_call_forwarding(sim)
        fwd_page.set_forward_when_unreachable(number)
        return fwd_page

    @autopilot.logging.log_action(logger.info)
    def unset_forward_when_unreachable(self, sim=None):
        """Disables forwarding when unreachable on a sim.

        :param sim: Number of what SIM to use, either 1 or 2.
            Required parameter in dual SIM setups
        """
        fwd_page = self._enter_call_forwarding(sim)
        fwd_page.unset_forward_when_unreachable()
        return fwd_page

    @autopilot.logging.log_action(logger.info)
    def get_forward_when_unreachable(self, sim=None):
        """Return forwarding when unreachable value on sim.

        :param sim: Number of what SIM to use, either 1 or 2.
            Required parameter in dual SIM setups
        :returns: The forward when unreachable value.
        """
        fwd_page = self._enter_call_forwarding(sim)
        return fwd_page.get_forward_when_unreachable()


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

    @autopilot.logging.log_action(logger.info)
    def _set_rule(self):
        """Saves rule."""
        button = self.wait_select_single('Button',
                                         objectName='setButton')
        self.pointing_device.click_object(button)

    @autopilot.logging.log_action(logger.info)
    def _check_rule(self, rule, value):
        """Checks a rule's associated CheckBox with value.

        :param rule: The string representation of the rule.
        :param value: The new value of the CheckBox.
        """
        check = self.wait_select_single(ubuntuuitoolkit.CheckBox,
                                        objectName='check_%s' % rule)
        if value:
            check.check()
        else:
            check.uncheck()

    @autopilot.logging.log_action(logger.info)
    def enable_rule(self, rule):
        """Enables a rule.

        :param rule: The string representation of the rule.
        """
        self._check_rule(rule, True)

    @autopilot.logging.log_action(logger.info)
    def disable_rule(self, rule):
        """Disables a rule.

        :param rule: The string representation of the rule.
        """
        self._check_rule(rule, False)

    @autopilot.logging.log_action(logger.info)
    def _enter_rule(self, rule, number):
        """Enters a number into a rule's associated TextField.

        :param rule: The string representation of the rule.
        :param number: Number to enter into field.
        """
        field = self.wait_select_single('TextField',
                                        objectName='field_%s' % rule)
        field.write(number)

    @autopilot.logging.log_action(logger.info)
    def _get_rule(self, rule):
        """Returns the value of the rule.

        :param rule: The string representation of the rule.
        """
        return self.wait_select_single(objectName='current_%s' % rule).value

    @autopilot.logging.log_action(logger.info)
    def set_forward_unconditionally(self, number):
        """Sets forward unconditionally.

        :param number: Number to forward to.
        """
        self.enable_rule('voiceUnconditional')
        self._enter_rule('voiceUnconditional', number)
        self._set_rule()

    @autopilot.logging.log_action(logger.info)
    def unset_forward_unconditionally(self):
        """Disables forward unconditionally."""
        self.disable_rule('voiceUnconditional')

    @autopilot.logging.log_action(logger.info)
    def get_forward_unconditionally(self):
        """Returns value of forward unconditionally."""
        return self._get_rule('voiceUnconditional')

    @autopilot.logging.log_action(logger.info)
    def set_forward_on_busy(self, number):
        """Sets forward on busy.

        :param number: Number to forward to.
        """
        self.enable_rule('voiceBusy')
        self._enter_rule('voiceBusy', number)
        self._set_rule()

    @autopilot.logging.log_action(logger.info)
    def unset_forward_on_busy(self):
        """Disables forward on busy."""
        self.disable_rule('voiceBusy')

    @autopilot.logging.log_action(logger.info)
    def get_forward_on_busy(self):
        """Returns value of forward on busy."""
        return self._get_rule('voiceBusy')

    @autopilot.logging.log_action(logger.info)
    def set_forward_when_no_answer(self, number):
        """Sets forward on no answer.

        :param number: Number to forward to.
        """
        self.enable_rule('voiceNoReply')
        self._enter_rule('voiceNoReply', number)
        self._set_rule()

    @autopilot.logging.log_action(logger.info)
    def unset_forward_when_no_answer(self):
        """Disables forward when no answer."""
        self.disable_rule('voiceNoReply')

    @autopilot.logging.log_action(logger.info)
    def get_forward_when_no_answer(self):
        """Returns value of forward on no answer."""
        return self._get_rule('voiceNoReply')

    @autopilot.logging.log_action(logger.info)
    def set_forward_when_unreachable(self, number):
        """Sets forward when unreachable.

        :param number: Number to forward to.
        """
        self.enable_rule('voiceNotReachable')
        self._enter_rule('voiceNotReachable', number)
        self._set_rule()

    @autopilot.logging.log_action(logger.info)
    def unset_forward_when_unreachable(self):
        """Disables forward when unreachable."""
        self.disable_rule('voiceNotReachable')

    @autopilot.logging.log_action(logger.info)
    def get_forward_when_unreachable(self):
        """Returns value of forward when unreachable."""
        return self._get_rule('voiceNotReachable')


class Services(ubuntuuitoolkit.UbuntuUIToolkitCustomProxyObjectBase):

    """Autopilot helper for the Sim Services page."""

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
        return self.get_root_instance().wait_select_single(
            objectName='resetLauncherDialog')

    def _wait_and_return_main_system_settins_page(self):
        main_view = self.get_root_instance().select_single(
            objectName='systemSettingsMainView')
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

    """Autopilot helper for the WiFi page."""

    @classmethod
    def validate_dbus_object(cls, path, state):
        name = introspection.get_classname_from_path(path)
        if name == b'PageComponent':
            if state['objectName'][1] == 'wifiPage':
                return True
        return False

    @autopilot.logging.log_action(logger.debug)
    def enable_wireless(self):
        self._set_wireless(True)

    @autopilot.logging.log_action(logger.debug)
    def disable_wireless(self):
        self._set_wireless(False)

    """
    :returns: Whether or not WiFi can be used
    """
    @autopilot.logging.log_action(logger.debug)
    def have_wireless(self):
        try:
            self.wait_select_single('SwitchMenu', text=_('Wi-Fi'))
        except StateNotFoundError:
            return False
        return True

    """Returns the current WiFi state

    :returns: Whether or not WiFi is enabled
    """
    @autopilot.logging.log_action(logger.debug)
    def get_wireless(self):
        return self.wait_select_single(
            'SwitchMenu', text=_('Wi-Fi')).checked

    @autopilot.logging.log_action(logger.debug)
    def _set_wireless(self, state):
        obj = self.wait_select_single('SwitchMenu', text=_('Wi-Fi'))
        if obj.checked != state:
            self.pointing_device.click_object(obj)

    """Connects to hidden network

    :param name: Network name string (SSID)
    :kwarg security: A string that is either "none", "wpa" or "wep
    :kwarg password: A string/hex secret
    :kwarg cancel: A boolean deciding whether we press cancel or not

    :returns: If we are connecting, it returns the dialog,
        if we cancel, it returns itself

    """
    @autopilot.logging.log_action(logger.debug)
    def connect_to_hidden_network(self, name, security='none', username=None,
                                  password=None, auth=None, protocol=None,
                                  cancel=False):
        dialog = self._click_connect_to_hidden_network()
        dialog._scroll_to_and_click = self._scroll_to_and_click
        dialog.enter_name(name)

        if security:
            dialog.set_security(security)
        if auth:
            dialog.set_auth(auth)
        if protocol:
            dialog.set_protocol(protocol)
        if username:
            dialog.enter_username(username)
        if password:
            dialog.enter_password(password)
        if cancel:
            dialog.cancel()
            return self
        else:
            dialog.connect()
            return dialog

    @autopilot.logging.log_action(logger.debug)
    def _click_connect_to_hidden_network(self):

        # we can't mock the qunitymenu items, so we
        # have to wait for them to be built
        sleep(0.5)

        button = self.select_single('*',
                                    objectName='connectToHiddenNetwork')
        self._scroll_to_and_click(button)
        return self.get_root_instance().wait_select_single(
            objectName='otherNetworkDialog')

    @autopilot.logging.log_action(logger.debug)
    def go_to_previous_networks(self):
        return self._click_previous_network()

    """Removes previous network

    :param ssid: Network name string (SSID)

    :returns: PreviousNetwork page

    """
    @autopilot.logging.log_action(logger.debug)
    def remove_previous_network(self, ssid):
        page = self.go_to_previous_networks()
        details = page.select_network(ssid)
        details.forget_network()
        return page

    @autopilot.logging.log_action(logger.debug)
    def _click_previous_network(self):

        # we can't mock the qunitymenu items, so we
        # have to wait for them to be built
        sleep(0.5)

        button = self.select_single('*',
                                    objectName='previousNetwork')
        self._scroll_to_and_click(button)
        return self.get_root_instance().wait_select_single(
            objectName='previousNetworksPage')


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

    # FIXME: Use ListItem methods instead.
    @autopilot.logging.log_action(logger.debug)
    def _expand_list(self, list_name):
        item_list = self.select_single(
            '*', objectName=list_name)
        active_child = item_list.select_single(
            '*', objectName='listContainer')
        self._scroll_to_and_click(active_child)
        # wait for it to expand
        sleep(0.5)
        return item_list

    @autopilot.logging.log_action(logger.debug)
    def enter_name(self, name):
        self._enter_name(name)

    @autopilot.logging.log_action(logger.debug)
    def _enter_name(self, name):
        namefield = self.select_single('TextField',
                                       objectName='networkname')
        self._scroll_to_and_click(namefield)
        namefield.write(name)

    @autopilot.logging.log_action(logger.debug)
    def enter_username(self, username):
        self._enter_username(username)

    @autopilot.logging.log_action(logger.debug)
    def _enter_username(self, username):
        namefield = self.select_single('TextField',
                                       objectName='username')
        self._scroll_to_and_click(namefield)
        namefield.write(username)

    @autopilot.logging.log_action(logger.debug)
    def set_security(self, security):
        """Sets the hidden network's security

        :param security: Either 'none', 'wpa', 'wep', 'wpa-ep', 'dewp', 'leap'

        :returns: None
        """

        # We only set security if not none, since none is default
        if security != 'none':
            self._set_security(security)

    @autopilot.logging.log_action(logger.debug)
    def _set_security(self, security):
        s_list = self._expand_list('securityList')
        item = None
        if security == 'none':
            item = s_list.wait_select_single('*', text=_('None'))
        elif security == 'wpa':
            item = s_list.wait_select_single('*',
                                             text=_('WPA & WPA2 Personal'))
        elif security == 'wep':
            item = s_list.wait_select_single('*', text=_('WEP'))
        elif security == 'wpa-ep':
            item = s_list.wait_select_single('*',
                                             text=_('WPA & WPA2 Enterprise'))
        elif security == 'dwep':
            item = s_list.wait_select_single('*',
                                             text=_('Dynamic WEP (802.1x)'))
        elif security == 'leap':
            item = s_list.wait_select_single('*', text=_('LEAP'))
        elif security is not None:
            raise ValueError('security type %s is not valid' % security)

        self.pointing_device.click_object(item)
        # wait for ui to change
        sleep(0.5)

    @autopilot.logging.log_action(logger.debug)
    def enter_password(self, password):
        self._enter_password(password)

    @autopilot.logging.log_action(logger.debug)
    def _enter_password(self, password):
        pwdfield = self.select_single('TextField',
                                      objectName='password')
        self._scroll_to_and_click(pwdfield)
        pwdfield.write(password)

    @autopilot.logging.log_action(logger.debug)
    def set_protocol(self, protocol):
        """Sets the hidden network's protocol.

        :param protocol: Either 'pap', 'mschapv2', 'mschap', 'chap', 'gtc',
                         or 'md5'.
        """
        self._set_protocol(protocol)

    @autopilot.logging.log_action(logger.debug)
    def _set_protocol(self, protocol):
        p_list = self._expand_list('p2authList')
        item = None
        if protocol == 'pap':
            item = p_list.wait_select_single(text='PAP')
        elif protocol == 'mschapv2':
            item = p_list.wait_select_single(text='MSCHAPv2')
        elif protocol == 'mschap':
            item = p_list.wait_select_single(text='MSCHAP')
        elif protocol == 'chap':
            item = p_list.wait_select_single(text='CHAP')
        elif protocol == 'gtc':
            item = p_list.wait_select_single(text='GTC')
        elif protocol == 'md5':
            item = p_list.wait_select_single(text='MD5')
        elif protocol is not None:
            raise ValueError('protocol type %s is not valid' % protocol)

        self.pointing_device.click_object(item)
        # wait for ui to change
        sleep(0.5)

    @autopilot.logging.log_action(logger.debug)
    def set_auth(self, auth):
        """Sets the hidden network's protocol.

        :param auth: Either 'tls', 'ttls', 'leap', 'fast' or 'peap'.
        """
        self._set_auth(auth)

    @autopilot.logging.log_action(logger.debug)
    def _set_auth(self, auth):
        a_list = self._expand_list('authList')
        item = None
        if auth == 'tls':
            item = a_list.wait_select_single(text='TLS')
        elif auth == 'ttls':
            item = a_list.wait_select_single(text='TTLS')
        elif auth == 'leap':
            item = a_list.wait_select_single(text='LEAP')
        elif auth == 'fast':
            item = a_list.wait_select_single(text='FAST')
        elif auth == 'peap':
            item = a_list.wait_select_single(text='PEAP')
        elif auth is not None:
            raise ValueError('auth type %s is not valid' % auth)

        self.pointing_device.click_object(item)
        # wait for ui to change
        sleep(0.5)

    @autopilot.logging.log_action(logger.debug)
    def cancel(self):
        self._click_cancel()

    @autopilot.logging.log_action(logger.debug)
    def _click_cancel(self):
        button = self.select_single('Button', objectName='cancel')
        self._scroll_to_and_click(button)

    @autopilot.logging.log_action(logger.debug)
    def connect(self):
        self._click_connect()

    @autopilot.logging.log_action(logger.debug)
    def _click_connect(self):
        button = self.select_single('Button', objectName='connect')
        self._scroll_to_and_click(button)


class PreviousNetworks(
        ubuntuuitoolkit.UbuntuUIToolkitCustomProxyObjectBase):

    """Autopilot helper for the Previous Networks page."""

    @classmethod
    def validate_dbus_object(cls, path, state):
        name = introspection.get_classname_from_path(path)
        if name == b'ItemPage':
            if state['objectName'][1] == 'previousNetworksPage':
                return True
        return False

    @autopilot.logging.log_action(logger.debug)
    def select_network(self, name):
        self._select_network(name)
        return self.get_root_instance().wait_select_single(
            objectName='networkDetailsPage')

    @autopilot.logging.log_action(logger.debug)
    def _select_network(self, name):
        net = self.select_single('Standard', text=name)
        self.pointing_device.click_object(net)


class NetworkDetails(
        ubuntuuitoolkit.UbuntuUIToolkitCustomProxyObjectBase):

    """Autopilot helper for the Networks Details page."""

    @autopilot.logging.log_action(logger.debug)
    def forget_network(self):
        self._click_forget()

    @autopilot.logging.log_action(logger.debug)
    def _click_forget(self):
        button = self.select_single('Button', objectName='forgetNetwork')
        self.pointing_device.click_object(button)
