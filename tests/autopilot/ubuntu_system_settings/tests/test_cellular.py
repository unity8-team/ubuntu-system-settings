# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2014 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.
import dbus
from time import sleep
from autopilot.introspection.dbus import StateNotFoundError
from testtools.matchers import Equals, NotEquals, raises

from ubuntu_system_settings.tests import UbuntuSystemSettingsOfonoTestCase
from ubuntu_system_settings.utils.i18n import ugettext as _

from ubuntuuitoolkit import emulators as toolkit_emulators


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
                objectName="chooseDataTypeSelector"
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

    def test_current_network(self):
        """ Tests whether the current network is visible and selected """
        self.navigate_to_manual()
        carriers = self.system_settings.main_view.choose_page.select_single(
            toolkit_emulators.ItemSelector,
            objectName="carrierSelector"
        )
        # TODO: Once there is a proper ItemSelector emulator, get the items
        # from it and check 'fake.tel' is the selected one.
        manual = carriers.select_single('Label', text="fake.tel")
        self.assertThat(manual, NotEquals(None))
        self.assertThat(carriers.selectedIndex, Equals(0))

    def test_alt_network(self):
        """ Tests whether an alternative available network is displayed """
        self.navigate_to_manual()
        carriers = self.system_settings.main_view.choose_page.select_single(
            toolkit_emulators.ItemSelector,
            objectName="carrierSelector"
        )
        manual = carriers.select_single('Label', text="my.cool.telco")
        self.assertThat(manual, NotEquals(None))

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

    def test_that_technology_UI_reflects_DBus(self):
        """DBus has been mocked and the 'any' preference is selected
        Assert that this is also selected in the UI
        """
        # assert that 'any' is selected
        self.assert_selected_preference(2)

    def test_off_setting_disables_roaming(self):
        """Test that switching off cellular data disables roaming switch"""
        roaming_switch = self.system_settings.main_view.select_single(
            '*', objectName="dataRoamingSwitch"
        )

        # select 2G only
        self.select_preference('2G only (saves battery)')
        self.assertTrue(roaming_switch.get_properties()['enabled'])

        # click off
        self.select_preference('Off')
        # make sure roaming_switch is disabled
        self.assertFalse(roaming_switch.get_properties()['enabled'])

    def test_modem_changes_reflected_in_UI(self):
        """Assert that DBus change to org.ofono.RadioSettings
        is reflected in the UI"""
        # fake dbus signal, changing to gsm
        self.modem_0.EmitSignal(
            'org.ofono.RadioSettings',
            'PropertyChanged',
            'sv',
            ['TechnologyPreference',  dbus.String('gsm', variant_level=1)])

        # assert that "2G" is selected
        self.assert_selected_preference(1)

    def test_that_technology_UI_is_not_inadvertently_changed_by_DBus(self):
        """Assert that the modem, changing DBus, cannot turn on
        cellular data if it previously was off"""
        # turn off cellular data
        self.select_preference('Off')

        # fake dbus signal, changing to gsm
        self.modem_0.EmitSignal(
            'org.ofono.RadioSettings',
            'PropertyChanged',
            'sv',
            ['TechnologyPreference',  dbus.String('gsm', variant_level=1)])

        # TODO: use 'eventually' instead
        sleep(1)

        # assert that "Off" has not changed
        self.assert_selected_preference(0)
