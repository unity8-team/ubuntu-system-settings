# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2014 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

import dbus
from time import sleep

from autopilot.introspection.dbus import StateNotFoundError
from autopilot.matchers import Eventually
from testtools.matchers import Equals, NotEquals, raises
from unittest import skip

from ubuntu_system_settings.tests import (
    CellularBaseTestCase, CONNMAN_IFACE, RDO_IFACE)
from ubuntu_system_settings.utils.i18n import ugettext as _

from ubuntuuitoolkit import emulators as toolkit_emulators


PREFERENCE_2G = '2G only (saves battery)'
PREFERENCE_ANY = '2G/3G/4G (faster)'
PREFERENCE_OFF = 'Off'
USE_OFF = "useOff"
USE_SIM_1 = "usesim1"
USE_SIM_2 = "usesim2"


class CellularTestCase(CellularBaseTestCase):
    """ Tests for cellular Page """

    """Caches the technology preference selector"""
    _pref_selector = None

    @property
    def data_preference_selector(self):
        """Return data_preference_selector"""
        try:
            self._pref_selector.get_properties()
        except:
            self._pref_selector = \
                self.system_settings.main_view.cellular_page.select_single(
                    toolkit_emulators.ItemSelector,
                    objectName="technologyPreferenceSelector"
                )
        return self._pref_selector

    def select_preference(self, label):
        """Helper method to click a preference which matches provided label"""
        pref = self.data_preference_selector.select_single('Label', text=label)
        self.system_settings.main_view.pointing_device.click_object(pref)

    def assert_selected_preference(self, index):
        """Helper method asserting that the selected data technology preference
        is that of index"""
        self.assertThat(self.data_preference_selector.selectedIndex,
                        Eventually(Equals(index)))

    def navigate_to_carrier_page(self):
        selector = self.system_settings.main_view.cellular_page.select_single(
            objectName="chooseCarrier"
        )
        self.system_settings.main_view.scroll_to_and_click(selector)

    def navigate_to_manual(self):
        selector = self.choose_carrier_page.select_single(
            toolkit_emulators.ItemSelector,
            objectName="autoChooseCarrierSelector"
        )
        manual = selector.select_single('Label', text=_("Manually"))
        self.system_settings.main_view.pointing_device.click_object(manual)
        choosecarrier = self.system_settings.main_view.cellular_page.\
            select_single(objectName="chooseCarrier")
        self.system_settings.main_view.pointing_device.click_object(
            choosecarrier)
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
        self.navigate_to_carrier_page()
        self.navigate_to_manual()
        carriers = self.system_settings.main_view.choose_page.select_single(
            toolkit_emulators.ItemSelector,
            objectName="carrierSelector"
        )
        # TODO: Once there is a proper ItemSelector emulator, get the items
        # from it and check 'fake.tel' is the selected one.
        selected_delegate = carriers.select_single(
            'OptionSelectorDelegate', selected=True)
        selected_delegate.select_single('Label', text="fake.tel")

    def test_alt_network(self):
        """ Tests whether an alternative available network is displayed """
        self.navigate_to_carrier_page()
        self.navigate_to_manual()
        carriers = self.system_settings.main_view.choose_page.select_single(
            toolkit_emulators.ItemSelector,
            objectName="carrierSelector"
        )
        manual = carriers.select_single('Label', text="my.cool.telco")
        self.assertThat(manual, NotEquals(None))

    def test_no_forbidden_network(self):
        """ Ensures that a forbidden network is not shown """
        self.navigate_to_carrier_page()
        self.navigate_to_manual()
        carriers = self.system_settings.main_view.choose_page.select_single(
            toolkit_emulators.ItemSelector,
            objectName="carrierSelector"
        )
        self.assertThat(
            lambda: carriers.select_single('Label', text="my.bad.telco"),
            raises(StateNotFoundError)
        )

    def test_set_sim_offline(self):
        self.select_preference(PREFERENCE_OFF)

        sleep(0.7)

        self.assertEqual(False, self.modem_0.Get(CONNMAN_IFACE, 'Powered'))

    def test_set_sim_online(self):
        self.select_preference(PREFERENCE_OFF)
        sleep(0.7)
        self.assertEqual(False, self.modem_0.Get(CONNMAN_IFACE, 'Powered'))

        self.select_preference(PREFERENCE_ANY)
        sleep(0.7)
        self.assertEqual(True, self.modem_0.Get(CONNMAN_IFACE, 'Powered'))

    def test_roaming_switch(self):
        """Test that switching off cellular data disables roaming switch"""
        roaming_switch = self.system_settings.main_view.select_single(
            objectName="dataRoamingSwitch"
        )
        # select 2G only
        self.select_preference(PREFERENCE_2G)

        # assert that roaming_switch is enabled
        self.assertThat(
            roaming_switch.get_properties()['enabled'],
            Eventually(Equals(True)))

        # click off
        self.select_preference(PREFERENCE_OFF)
        # assert roaming_switch is disabled
        self.assertThat(
            roaming_switch.get_properties()['enabled'],
            Eventually(Equals(False)))

    def test_allow_roaming(self):
        roaming_switch = self.system_settings.main_view.select_single(
            objectName="dataRoamingSwitch"
        )
        self.system_settings.main_view.scroll_to_and_click(roaming_switch)
        sleep(1.5)
        self.assertEqual(
            True, self.modem_0.Get(CONNMAN_IFACE, 'RoamingAllowed'))

    def test_change_data_preference(self):
        self.select_preference(PREFERENCE_2G)
        sleep(0.7)
        self.assertEqual('gsm', self.modem_0.Get(RDO_IFACE,
                                                 'TechnologyPreference'))
        self.select_preference(PREFERENCE_ANY)
        sleep(0.7)
        self.assertEqual('any', self.modem_0.Get(RDO_IFACE,
                                                 'TechnologyPreference'))

    def test_sim_online_status_insensitive_to_radio_preference(self):
        # turn off cellular data
        self.select_preference(PREFERENCE_OFF)

        # fake dbus signal, changing to any
        self.modem_0.EmitSignal(
            'org.ofono.RadioSettings',
            'PropertyChanged',
            'sv',
            ['TechnologyPreference',  dbus.String('any', variant_level=1)])

        # assert that "Off" has not changed
        self.assert_selected_preference(0)

    def test_ui_reacts_to_sim_set_coming_online(self):
        self.select_preference(PREFERENCE_2G)
        self.select_preference(PREFERENCE_OFF)

        sleep(0.7)

        self.modem_0.EmitSignal(
            CONNMAN_IFACE,
            'PropertyChanged',
            'sv',
            ['Powered', 'true'])

        # assert that 2G is selected
        self.assert_selected_preference(1)

    def test_radio_preference_change_does_not_override_user_selection(self):
        self.select_preference(PREFERENCE_2G)

        self.modem_0.EmitSignal(
            'org.ofono.RadioSettings',
            'PropertyChanged',
            'sv',
            ['TechnologyPreference',  dbus.String('lte', variant_level=1)])

        self.assertEqual('gsm', self.modem_0.Get(RDO_IFACE,
                                                 'TechnologyPreference'))

        # assert that the preference is any
        self.assert_selected_preference(1)

    def test_unlocking_sim(self):
        '''Like it would if the sim was locked, e.g.'''
        self.modem_0.Set(RDO_IFACE, 'TechnologyPreference',
                         dbus.String('', variant_level=1))

        self.system_settings.main_view.go_back()

        self.system_settings.main_view.pointing_device.click_object(
            self.system_settings.main_view.select_single(
                objectName='entryComponent-cellular'))

        self.assert_selected_preference(-1)
        self.assertFalse(self.data_preference_selector.enabled)

        self.modem_0.EmitSignal(
            'org.ofono.RadioSettings',
            'PropertyChanged',
            'sv',
            ['TechnologyPreference',  dbus.String('lte', variant_level=1)])

        self.assert_selected_preference(2)

        def test_that_sim_editor_is_hidden(self):
            pass


class DualSimCellularTestCase(CellularBaseTestCase):

    use_sims = 2

    def navigate_to_carriers_page(self):
        selector = self.system_settings.main_view.cellular_page.select_single(
            objectName="chooseCarrier"
        )
        self.system_settings.main_view.scroll_to_and_click(selector)

    def navigate_to_carrier_page_for_sim(self, n):
        selector = self.choose_carriers_page.select_single(
            objectName="chooseCarrierSim%d" % n
        )
        self.system_settings.main_view.scroll_to_and_click(selector)

    def navigate_to_manual(self):
        selector = self.choose_carrier_page.select_single(
            toolkit_emulators.ItemSelector,
            objectName="autoChooseCarrierSelector"
        )
        manual = selector.select_single('Label', text=_("Manually"))
        self.system_settings.main_view.scroll_to_and_click(manual)
        choosecarrier = self.system_settings.main_view.cellular_page.\
            select_single(objectName="chooseCarrier")
        self.system_settings.main_view.scroll_to_and_click(choosecarrier)
        self.assertThat(
            self.system_settings.main_view.choose_page.title,
            Equals(_("Carrier"))
        )

    @property
    def data_preference_selector(self):
        """Return data_preference_selector"""
        try:
            self._pref_selector.get_properties()
        except:
            self._pref_selector = \
                self.system_settings.main_view.cellular_page.select_single(
                    toolkit_emulators.ItemSelector,
                    objectName="technologyPreferenceSelector"
                )
        return self._pref_selector

    def select_preference(self, label):
        """Helper method that clicks a preference
        that matches provided label"""
        pref = self.data_preference_selector.select_single('Label', text=label)
        self.system_settings.main_view.scroll_to_and_click(pref)

    def assert_selected_preference(self, index):
        """Helper method asserting that the selected
        data technology preference is that of index"""
        self.assertThat(
            self.data_preference_selector.selectedIndex,
            Eventually(Equals(index)))

    def use_selector(self, label):
        obj = self.system_settings.main_view.cellular_page.select_single(
            objectName="use"
        ).select_single(objectName=label)
        self.system_settings.main_view.scroll_to_and_click(obj)

    def assert_used(self, index):
        obj = self.system_settings.main_view.cellular_page.select_single(
            objectName="use"
        )
        self.assertThat(
            obj.selectedIndex, Eventually(Equals(index)))

    def use_sim(self, num):
        """Manipulate use selector"""


    def test_use_sim_1(self):
        self.use_selector(USE_OFF)
        self.use_selector(USE_SIM_1)
        sleep(0.7)
        self.assertEqual(True, self.modem_0.Get(CONNMAN_IFACE, 'Powered'))
        self.assertEqual(False, self.modem_1.Get(CONNMAN_IFACE, 'Powered'))

    def test_use_sim_2(self):
        self.use_selector(USE_OFF)
        self.use_selector(USE_SIM_2)
        sleep(0.7)
        self.assertEqual(False, self.modem_0.Get(CONNMAN_IFACE, 'Powered'))
        self.assertEqual(True, self.modem_1.Get(CONNMAN_IFACE, 'Powered'))

    def test_turn_off_both_sims(self):
        self.use_selector(USE_OFF)
        sleep(0.7)
        self.assertEqual(False, self.modem_0.Get(CONNMAN_IFACE, 'Powered'))
        self.assertEqual(False, self.modem_1.Get(CONNMAN_IFACE, 'Powered'))

    def test_use_gsm_for_sim_1(self):
        self.use_selector(USE_SIM_1)
        self.select_preference(PREFERENCE_2G)
        sleep(0.7)
        self.assertEqual(
            'gsm', self.modem_0.Get(RDO_IFACE, 'TechnologyPreference'))

    def test_use_any_for_sim_1(self):
        self.use_selector(USE_SIM_1)
        self.select_preference(PREFERENCE_ANY)
        sleep(0.7)
        self.assertEqual(
            'any', self.modem_0.Get(RDO_IFACE, 'TechnologyPreference'))

    def test_use_gsm_for_sim_2(self):
        self.use_selector(USE_SIM_1)
        self.select_preference(PREFERENCE_2G)
        sleep(0.7)
        self.assertEqual(
            'gsm', self.modem_0.Get(RDO_IFACE, 'TechnologyPreference'))

    def test_use_any_for_sim_2(self):
        self.use_selector(USE_SIM_2)
        self.select_preference(PREFERENCE_ANY)
        sleep(1)
        self.assertEqual(
            'any', self.modem_1.Get(RDO_IFACE, 'TechnologyPreference'))

    def test_when_sim_1_comes_online_ui_is_correct(self):
        self.use_selector(USE_SIM_1)
        self.select_preference(PREFERENCE_ANY)
        self.use_selector(USE_OFF)
        sleep(0.7)
        self.modem_0.Set(CONNMAN_IFACE, 'Powered', True)
        self.modem_0.EmitSignal(
            CONNMAN_IFACE,
            'PropertyChanged',
            'sv',
            ['Powered', 'true'])

        self.assertEqual(
            'any', self.modem_0.Get(RDO_IFACE, 'TechnologyPreference'))
        self.assert_used(1)
        self.assert_selected_preference(1)

    def test_when_sim_2_comes_online_ui_is_correct(self):
        self.use_selector(USE_SIM_2)
        self.select_preference(PREFERENCE_ANY)
        self.use_selector(USE_OFF)
        sleep(0.7)
        self.modem_1.Set(CONNMAN_IFACE, 'Powered', True)
        self.modem_1.EmitSignal(
            CONNMAN_IFACE,
            'PropertyChanged',
            'sv',
            ['Powered', 'true'])

        self.assertEqual(
            'any', self.modem_1.Get(RDO_IFACE, 'TechnologyPreference'))
        self.assert_used(2)
        self.assert_selected_preference(1)

    def test_roaming_switch(self):
        roaming_switch = self.system_settings.main_view.select_single(
            objectName="dataRoamingSwitch"
        )
        # assert that roaming_switch is enabled
        self.assertTrue(roaming_switch.get_properties()['enabled'])

        # click off
        self.use_selector(USE_OFF)

        # assert roaming_switch is disabled
        self.assertThat(
            roaming_switch.get_properties()['enabled'],
            Eventually(Equals(False)))

    def test_allow_roaming(self):
        self.use_selector(USE_SIM_1)
        self.assertEqual(
            False, self.modem_0.Get(CONNMAN_IFACE, 'RoamingAllowed'))
        roaming_switch = self.system_settings.main_view.select_single(
            objectName="dataRoamingSwitch"
        )
        self.system_settings.main_view.scroll_to_and_click(roaming_switch)
        sleep(1.5)
        self.assertEqual(
            True, self.modem_0.Get(CONNMAN_IFACE, 'RoamingAllowed'))

    def test_no_radio_preference(self):
        self.select_preference(PREFERENCE_ANY)
        self.use_selector(USE_OFF)

        self.modem_0.Set(RDO_IFACE, 'TechnologyPreference', '')
        self.modem_0.EmitSignal(
            CONNMAN_IFACE,
            'PropertyChanged',
            'sv',
            ['TechnologyPreference', ''])

        self.assertThat(
            self.data_preference_selector.get_properties()['enabled'],
            Eventually(Equals(False)))

    def test_radio_preference_changes(self):
        self.use_selector(USE_SIM_1)

        self.modem_0.Set(RDO_IFACE, 'TechnologyPreference', 'any')
        self.modem_0.EmitSignal(
            RDO_IFACE,
            'PropertyChanged',
            'sv',
            ['TechnologyPreference', 'any'])

        self.assert_selected_preference(1)

    # see
    # https://gitorious.org/python-dbusmock/python-dbusmock/merge_requests/3
    @skip('skipped due to bug in dbusmock')
    def test_change_op_sim_1(self):
        self.navigate_to_carriers_page()
        self.navigate_to_carrier_page_for_sim(1)
        carriers = self.system_settings.main_view.choose_page.select_single(
            toolkit_emulators.ItemSelector,
            objectName="carrierSelector"
        )
        manual = carriers.select_single('Label', text="my.cool.telco")
        self.assertThat(manual, NotEquals(None))

    # see
    # https://gitorious.org/python-dbusmock/python-dbusmock/merge_requests/3
    @skip('skipped due to bug in dbusmock')
    def test_change_op_sim_2(self):
        self.navigate_to_carriers_page()
        self.navigate_to_carrier_page_for_sim(2)
        carriers = self.system_settings.main_view.choose_page.select_single(
            toolkit_emulators.ItemSelector,
            objectName="carrierSelector"
        )
        manual = carriers.select_single('Label', text="my.cool.telco")
        self.assertThat(manual, NotEquals(None))

    # def test_two_modems(self):
    #     sleep(9)
    #     self.assertFalse(True)

    # def test_changing_sim1_name(self):
    #     pass

    # def test_changing_sim2_name(self):
    #     pass

    # def test_changes_to_sim_names_in_gsettings_are_reflected_in_ui(self):
    #     pass

    # def test_changing_sim_name_to_empty(self):
    #     pass

    # def test_sim_name_when_no_number_or_SIMS(self):
    #     pass

    # def test_changing_sim_name_to_something_weird(self):
    #     pass


