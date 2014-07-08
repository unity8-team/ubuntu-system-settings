# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2014 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.
import dbus
from time import sleep
from autopilot.introspection.dbus import StateNotFoundError
from gi.repository import Gio
from testtools.matchers import Equals, NotEquals, raises

from ubuntu_system_settings.tests import (
    UbuntuSystemSettingsOfonoTestCase, CONNECTION_MANAGER_IFACE, RADIOSETTINGS_IFACE)
from ubuntu_system_settings.utils.i18n import ugettext as _

from ubuntuuitoolkit import emulators as toolkit_emulators


PREFERENCE_2G = '2G only (saves battery)'
PREFERENCE_ANY = '2G/3G/4G (faster)'
PREFERENCE_OFF = 'Off'


class CellularTestCase(UbuntuSystemSettingsOfonoTestCase):
    """ Tests for cellular Page """

    """Caches the technology preference selector"""
    _pref_selector = None

    @property
    def data_preference_selector(self):
        """Return data_preference_selector"""
        try:
            self._pref_selector.get_properties()
        except:
            self._pref_selector = self.system_settings.main_view.cellular_page.select_single(
                toolkit_emulators.ItemSelector,
                objectName="technologyPreferenceSelector"
            )
        return self._pref_selector

    def navigate_to_manual(self):
        selector = self.system_settings.main_view.cellular_page.select_single(
            toolkit_emulators.ItemSelector,
            objectName="autoChooseCarrierSelector"
        )
        manual = selector.select_single('Label', text=_("Manually"))
        self.system_settings.main_view.pointer.click_object(manual)
        choosecarrier = self.system_settings.main_view.cellular_page.\
            select_single(objectName="chooseCarrier")
        self.system_settings.main_view.pointer.click_object(choosecarrier)
        self.assertThat(
            self.system_settings.main_view.choose_page.title,
            Equals(_("Carrier"))
        )

    def test_cellular_page(self):
        """ Checks whether Cellular page is available """
        self.assertThat(
            self.system_settings.main_view.cellular_page,
            NotEquals(None)
        )
        self.assertThat(
            self.system_settings.main_view.cellular_page.title,
            Equals(_('Cellular'))
        )

    # def test_current_network(self):
    #     """ Tests whether the current network is visible and selected """
    #     self.navigate_to_manual()
    #     carriers = self.system_settings.main_view.choose_page.select_single(
    #         toolkit_emulators.ItemSelector,
    #         objectName="carrierSelector"
    #     )
    #     # TODO: Once there is a proper ItemSelector emulator, get the items
    #     # from it and check 'fake.tel' is the selected one.
    #     manual = carriers.select_single('Label', text="fake.tel")
    #     self.assertThat(manual, NotEquals(None))
    #     self.assertThat(carriers.selectedIndex, Equals(0))

    # def test_alt_network(self):
    #     """ Tests whether an alternative available network is displayed """
    #     self.navigate_to_manual()
    #     carriers = self.system_settings.main_view.choose_page.select_single(
    #         toolkit_emulators.ItemSelector,
    #         objectName="carrierSelector"
    #     )
    #     manual = carriers.select_single('Label', text="my.cool.telco")
    #     self.assertThat(manual, NotEquals(None))

    def test_no_forbidden_network(self):
        """ Ensures that a forbidden network is not shown """
        self.navigate_to_manual()
        carriers = self.system_settings.main_view.choose_page.select_single(
            toolkit_emulators.ItemSelector,
            objectName="carrierSelector"
        )
        self.assertThat(
            lambda: carriers.select_single('Label', text="my.bad.telco"),
            raises(StateNotFoundError)
        )

    def select_preference(self, label):
        """Helper method that clicks a preference that matches provided label"""
        pref = self.data_preference_selector.select_single('Label', text=label)
        self.system_settings.main_view.pointer.click_object(pref)

    def assert_selected_preference(self, index):
        """Helper method asserting that the selected data technology preference
        is that of index"""
        self.assertThat(self.data_preference_selector.selectedIndex, Equals(index))

    def test_turn_off_modem(self):
        self.select_preference(PREFERENCE_OFF)

        sleep(1)

        self.assertEqual(False, self.modem_0.Get(CONNECTION_MANAGER_IFACE, 'Powered'))


    def test_off_setting_disables_roaming(self):
        """Test that switching off cellular data disables roaming switch"""
        roaming_switch = self.system_settings.main_view.select_single(
            '*', objectName="dataRoamingSwitch"
        )

        # select 2G only
        self.select_preference(PREFERENCE_2G)
        # assert that roaming_switch is enabled
        self.assertTrue(roaming_switch.get_properties()['enabled'])

        # click off
        self.select_preference(PREFERENCE_OFF)
        # assert roaming_switch is disabled
        self.assertFalse(roaming_switch.get_properties()['enabled'])

    def test_changes_to_technology_preference_is_reflected_by_DBus(self):
        self.select_preference(PREFERENCE_2G)

        sleep(1)
        self.assertEqual('gsm', self.modem_0.Get(RADIOSETTINGS_IFACE, 'TechnologyPreference'))

        self.select_preference(PREFERENCE_ANY)

        sleep(1)

        self.assertEqual('any', self.modem_0.Get(RADIOSETTINGS_IFACE, 'TechnologyPreference'))

    def test_that_technology_UI_is_not_inadvertently_changed_by_DBus(self):
        """Assert that if the modem, if turned off, cannot be turned on
        by a change in technology preference"""

        # turn off cellular data
        self.select_preference(PREFERENCE_OFF)

        # fake dbus signal, changing to any
        self.modem_0.EmitSignal(
            'org.ofono.RadioSettings',
            'PropertyChanged',
            'sv',
            ['TechnologyPreference',  dbus.String('any', variant_level=1)])

        # TODO: use 'eventually' instead
        sleep(1)

        # assert that "Off" has not changed
        self.assert_selected_preference(0)

    def test_that_technology_UI_reflects_setting_when_modem_is_powered_up(self):
        """Assert that when modem comes online, UI should reflect this"""

        self.select_preference(PREFERENCE_2G)
        self.select_preference(PREFERENCE_OFF)

        sleep(1)

        self.modem_0.EmitSignal(
            CONNECTION_MANAGER_IFACE,
            'PropertyChanged',
            'sv',
            ['Powered', 'true'])

        sleep(1)

        # assert that 2G is selected
        self.assert_selected_preference(1)

    def test_that_lte_and_umts_are_equal_to_any(self):

        self.modem_0.EmitSignal(
            'org.ofono.RadioSettings',
            'PropertyChanged',
            'sv',
            ['TechnologyPreference',  dbus.String('lte', variant_level=1)])

        sleep(1)

        self.assertEqual('any', self.modem_0.Get(RADIOSETTINGS_IFACE, 'TechnologyPreference'))

        # assert that the preference is any
        self.assert_selected_preference(2)

    def test_that_technology_UI_is_sensitive_to_radiosettings_interface(self):
        """Assert that changes the radiosetting interface will be reflected in the UI.
        The reason to test this is that if the SIM is locked, the interface is absent.
        When unlocked, it becomes available and should trigger change in UI.
        """

        # remove radio settings interface
        # this simulates locking of SIM
        modem_interfaces = self.modem_0.GetProperties()['Interfaces']
        modem_interfaces.remove(RADIOSETTINGS_IFACE)
        self.modem_0.SetProperty('Interfaces', modem_interfaces)

        sleep(1)

        # since the radiosettings interface is gone, the tech
        # pref selector is disabled
        self.assertFalse(self.data_preference_selector.enabled)

        # add the radio settings interface
        modem_interfaces = self.modem_0.GetProperties()['Interfaces']
        modem_interfaces.append(RADIOSETTINGS_IFACE)
        self.modem_0.SetProperty('Interfaces', modem_interfaces)

        sleep(1)

        # assert that the preference is GSM
        self.assert_selected_preference(1)
