# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2014 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

# import dbus
from gi.repository import Gio, GLib
from time import sleep

from autopilot.introspection.dbus import StateNotFoundError
from autopilot.matchers import Eventually
from testtools.matchers import Equals, NotEquals, raises
from unittest import skip

from ubuntu_system_settings.tests import (
    CellularBaseTestCase, CONNMAN_IFACE, RDO_IFACE, SIM_IFACE, NETREG_IFACE)
# from ubuntu_system_settings.utils.i18n import ugettext as _

from ubuntuuitoolkit import emulators as toolkit_emulators


PREFERENCE_2G = '2G only (saves battery)'
PREFERENCE_UMTS = '2G/3G (faster)'
PREFERENCE_LTE = '2G/3G/4G (faster)'
PREFERENCE_OFF = 'Off'
USE_OFF = "useoff"
USE_SIM_1 = "use/ril_0"
USE_SIM_2 = "use/ril_1"


class CellularTestCase(CellularBaseTestCase):

    def test_enable_data(self):
        self.cellular_page.enable_data()
        self.assertThat(
            lambda: self.modem_0.Get(CONNMAN_IFACE, 'Powered'),
            Eventually(Equals(True))
        )

    def test_disable_data(self):
        self.cellular_page.disable_data()
        self.assertThat(
            lambda: self.modem_0.Get(CONNMAN_IFACE, 'Powered'),
            Eventually(Equals(False))
        )

    def test_remote_manipulation_of_data(self):
        self.modem_0.EmitSignal(
            CONNMAN_IFACE,
            'PropertyChanged',
            'sv',
            ['Powered', 'true'])

        self.assertThat(lambda: self.cellular_page.get_data(),
                        Eventually(Equals(True)))

        self.modem_0.EmitSignal(
            CONNMAN_IFACE,
            'PropertyChanged',
            'sv',
            ['Powered', 'false'])

        self.assertThat(lambda: self.cellular_page.get_data(),
                        Eventually(Equals(False)))

    def test_enable_roaming(self):
        self.cellular_page.enable_roaming()
        self.assertThat(
            lambda: self.modem_0.Get(CONNMAN_IFACE, 'RoamingAllowed'),
            Eventually(Equals(True))
        )

    def test_disable_roaming(self):
        self.cellular_page.disable_roaming()
        self.assertThat(
            lambda: self.modem_0.Get(CONNMAN_IFACE, 'RoamingAllowed'),
            Eventually(Equals(False))
        )

    def test_connection_type(self):
        get_pref = lambda: self.modem_0.Get(RDO_IFACE, 'TechnologyPreference')
        for pref in ['lte', 'umts', 'gsm']:
            self.cellular_page.set_connection_type(pref)
            self.assertThat(get_pref, Eventually(Equals(pref)))

    def test_current_carrier(self):
        self.assertThat(self.cellular_page.get_carrier(), Equals('fake.tel'))
        self.assertThat(lambda: self.modem_0.Get(NETREG_IFACE, 'Name'),
                        Eventually(Equals('fake.tel')))

    def test_change_carrier(self):
        self.cellular_page.change_carrier('my.cool.telco')
        self.assertThat(self.cellular_page.get_carrier(),
                        Equals('my.cool.telco'))

    def test_no_forbidden_network(self):
        """ Ensures that a forbidden network is not shown """
        self.assertThat(
            lambda: self.cellular_page.change_carrier('my.bad.telco'),
            raises(StateNotFoundError)
        )


class DualSimCellularTestCase(CellularBaseTestCase):

    use_sims = 2

    def test_data_off(self):
        self.cellular_page.select_sim_for_data('off')

    def get_sim_name(self, num):
        obj = self.system_settings.main_view.cellular_page.select_single(
            objectName="simLabel%d" % num)
        return obj.get_properties()['text']

    def rename_sim(self, num, new_name):
        obj = self.system_settings.main_view.cellular_page.select_single(
            objectName="simEditor"
        ).select_single(objectName="editSim%d" % num)
        self.system_settings.main_view.scroll_to_and_click(obj)
        field = self.system_settings.main_view.cellular_page.select_single(
            objectName="nameField"
        )
        self.system_settings.main_view.scroll_to_and_click(field)
        self.system_settings.main_view.scroll_to_and_click(
            field.select_single(objectName="clear_button"))
        self.keyboard.type(new_name)
        self.system_settings.main_view.scroll_to_and_click(
            self.system_settings.main_view.cellular_page.select_single(
                objectName="doRename"))

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

    def test_use_umts_for_sim_1(self):
        self.use_selector(USE_SIM_1)
        self.select_preference(PREFERENCE_UMTS)
        sleep(0.7)
        self.assertEqual(
            'umts', self.modem_0.Get(RDO_IFACE, 'TechnologyPreference'))

    def test_use_gsm_for_sim_2(self):
        self.use_selector(USE_SIM_1)
        self.select_preference(PREFERENCE_2G)
        sleep(0.7)
        self.assertEqual(
            'gsm', self.modem_0.Get(RDO_IFACE, 'TechnologyPreference'))

    def test_when_sim_1_comes_online_ui_is_correct(self):
        self.use_selector(USE_SIM_1)
        self.select_preference(PREFERENCE_UMTS)
        self.use_selector(USE_OFF)
        sleep(0.7)
        self.modem_0.Set(CONNMAN_IFACE, 'Powered', True)
        self.modem_0.EmitSignal(
            CONNMAN_IFACE,
            'PropertyChanged',
            'sv',
            ['Powered', 'true'])

        self.assertEqual(
            'umts', self.modem_0.Get(RDO_IFACE, 'TechnologyPreference'))
        self.assert_used(1)
        self.assert_selected_preference(1)

    def test_when_sim_2_comes_online_ui_is_correct(self):
        self.use_selector(USE_SIM_2)
        self.select_preference(PREFERENCE_2G)
        self.use_selector(USE_OFF)
        sleep(2)
        self.modem_1.Set(CONNMAN_IFACE, 'Powered', True)
        self.modem_1.EmitSignal(
            CONNMAN_IFACE,
            'PropertyChanged',
            'sv',
            ['Powered', 'true'])

        self.assertEqual(
            'gsm', self.modem_1.Get(RDO_IFACE, 'TechnologyPreference'))
        self.assert_used(2)
        self.assert_selected_preference(0)

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
        self.select_preference(PREFERENCE_UMTS)
        self.use_selector(USE_OFF)

        self.modem_0.Set(RDO_IFACE, 'TechnologyPreference', '')
        self.modem_0.EmitSignal(
            CONNMAN_IFACE,
            'PropertyChanged',
            'sv',
            ['TechnologyPreference', ''])

        self.assertThat(
            self.data_preference_selector.get_properties()['visible'],
            Eventually(Equals(False)))

    # see
    # https://gitorious.org/python-dbusmock/python-dbusmock/merge_requests/3
    @skip('skipped due to bug in dbusmock')
    def test_change_op_sim_1(self):
        self.navigate_to_carriers_page()
        self.navigate_to_carrier_page_for_sim(1)
        carriers = self.system_settings.main_view.choose_page.select_single(
            toolkit_emulators.ItemSelector, objectName="carrierSelector")
        manual = carriers.select_single('Label', text="my.cool.telco")
        self.assertThat(manual, NotEquals(None))

    # see
    # https://gitorious.org/python-dbusmock/python-dbusmock/merge_requests/3
    @skip('skipped due to bug in dbusmock')
    def test_change_op_sim_2(self):
        self.navigate_to_carriers_page()
        self.navigate_to_carrier_page_for_sim(2)
        carriers = self.system_settings.main_view.choose_page.select_single(
            toolkit_emulators.ItemSelector, objectName="carrierSelector")
        manual = carriers.select_single('Label', text="my.cool.telco")
        self.assertThat(manual, NotEquals(None))

    def test_radio_preference_changes(self):
        self.use_selector(USE_SIM_1)

        self.modem_0.Set(RDO_IFACE, 'TechnologyPreference', 'umts')
        self.modem_0.EmitSignal(
            RDO_IFACE,
            'PropertyChanged',
            'sv',
            ['TechnologyPreference', 'umts'])

        self.assert_selected_preference(1)

    def test_changing_sim1_name(self):
        gsettings = Gio.Settings.new('com.ubuntu.phone')
        old_name = gsettings.get_value('sim-names')['/ril_0']
        new_name = 'FOO BAR'
        self.rename_sim(1, new_name)

        # wait for gsettings
        sleep(1)
        try:
            self.assertEqual(
                new_name, gsettings.get_value('sim-names')['/ril_0'])
        except Exception as e:
            raise e
        finally:
            self.rename_sim(1, old_name)
            sleep(1)

    def test_changing_sim2_name(self):
        gsettings = Gio.Settings.new('com.ubuntu.phone')
        old_name = gsettings.get_value('sim-names')['/ril_1']
        new_name = 'BAR BAZ'
        self.rename_sim(2, new_name)

        # wait for gsettings
        sleep(1)
        try:
            self.assertEqual(
                new_name, gsettings.get_value('sim-names')['/ril_1'])
        except Exception as e:
            raise e
        finally:
            self.rename_sim(2, old_name)
            # wait for gsettings
            sleep(1)

    def test_changes_to_sim_names_in_gsettings_are_reflected_in_ui(self):
        gsettings = Gio.Settings.new('com.ubuntu.phone')
        old_names = gsettings.get_value('sim-names')

        new_names = old_names.unpack()
        new_names['/ril_0'] = 'BAS QUX'
        new_names = GLib.Variant('a{ss}', new_names)
        gsettings.set_value('sim-names', new_names)

        # wait for gsettings
        sleep(1)
        try:
            self.assertIn(new_names['/ril_0'], self.get_sim_name(1))
        except Exception as e:
            raise e
        finally:
            gsettings.set_value('sim-names', old_names)
            # wait for gsettings
            sleep(1)

    def test_changing_default_sim_for_calls(self):
        gsettings = Gio.Settings.new('com.ubuntu.phone')
        default = gsettings.get_value('default-sim-for-calls')

        self.addCleanup(
            self.set_default_for_calls, gsettings, default)

        # click ask
        self.system_settings.main_view.scroll_to_and_click(
            self.get_default_sim_for_calls_selector('ask'))
        # click first sim
        self.system_settings.main_view.scroll_to_and_click(
            self.get_default_sim_for_calls_selector('/ril_0'))
        # wait for gsettings
        sleep(1)
        self.assertEqual(
            gsettings.get_value('default-sim-for-calls').get_string(),
            '/ril_0')

    def test_changing_default_sim_for_messages(self):
        gsettings = Gio.Settings.new('com.ubuntu.phone')
        default = gsettings.get_value('default-sim-for-messages')
        self.addCleanup(
            self.set_default_for_messages, gsettings, default)

        # click ask
        self.system_settings.main_view.scroll_to_and_click(
            self.get_default_sim_for_messages_selector('ask'))
        # click second sim
        self.system_settings.main_view.scroll_to_and_click(
            self.get_default_sim_for_messages_selector('/ril_1'))
        # wait for gsettings
        sleep(1)
        self.assertEqual(
            gsettings.get_value('default-sim-for-messages').get_string(),
            '/ril_1')

    def test_multi_sim_layout(self):
        self.system_settings.main_view.cellular_page.\
            select_single(objectName="multiSim")
        self.assertThat(lambda: self.system_settings.main_view.select_single(
            objectName='singleSim'), raises(StateNotFoundError))
        self.assertThat(lambda: self.system_settings.main_view.select_single(
            objectName='noSim'), raises(StateNotFoundError))

    def test_remove_one_sim(self):
        self.modem_0.EmitSignal(
            SIM_IFACE,
            'PropertyChanged',
            'sv',
            ['Present', 'False'])

        self.system_settings.main_view.cellular_page.\
            select_single(objectName="singleSim")
        self.assertThat(lambda: self.system_settings.main_view.select_single(
            objectName='multiSim'), raises(StateNotFoundError))
        self.assertThat(lambda: self.system_settings.main_view.select_single(
            objectName='noSim'), raises(StateNotFoundError))

    def test_remove_two_sims(self):
        self.modem_0.EmitSignal(
            SIM_IFACE,
            'PropertyChanged',
            'sv',
            ['Present', 'False'])

        self.modem_1.EmitSignal(
            SIM_IFACE,
            'PropertyChanged',
            'sv',
            ['Present', 'False'])

        self.system_settings.main_view.cellular_page.\
            wait_select_single(objectName="noSim")
        self.assertThat(
            lambda: self.system_settings.main_view.select_single(
                objectName='multiSim'), raises(StateNotFoundError))
        self.assertThat(
            lambda: self.system_settings.main_view.select_single(
                objectName='singleSim'), raises(StateNotFoundError))

    def test_remove_and_insert_sims(self):
        self.modem_0.EmitSignal(
            SIM_IFACE,
            'PropertyChanged',
            'sv',
            ['Present', 'False'])

        self.modem_1.EmitSignal(
            SIM_IFACE,
            'PropertyChanged',
            'sv',
            ['Present', 'False'])

        self.system_settings.main_view.cellular_page.\
            wait_select_single(objectName="noSim")
        self.assertThat(
            lambda: self.system_settings.main_view.select_single(
                objectName='multiSim'), raises(StateNotFoundError))
        self.assertThat(
            lambda: self.system_settings.main_view.select_single(
                objectName='singleSim'), raises(StateNotFoundError))

        self.modem_0.EmitSignal(
            SIM_IFACE,
            'PropertyChanged',
            'sv',
            ['Present', 'True'])

        self.modem_1.EmitSignal(
            SIM_IFACE,
            'PropertyChanged',
            'sv',
            ['Present', 'True'])

        self.system_settings.main_view.cellular_page.\
            wait_select_single(objectName="multiSim")
        self.assertThat(
            lambda: self.system_settings.main_view.select_single(
                objectName='noSim'), raises(StateNotFoundError))
        self.assertThat(
            lambda: self.system_settings.main_view.select_single(
                objectName='singleSim'), raises(StateNotFoundError))

    # regression test for 1375832
    # tests that the second slot only exposes gsm, which
    # the testdata indicates
    def test_slot_two(self):
        self.modem_0.EmitSignal(
            SIM_IFACE,
            'PropertyChanged',
            'sv',
            ['Present', 'False'])
        self.select_preference(PREFERENCE_2G)
        self.assertRaises(StateNotFoundError,
                          self.select_preference, PREFERENCE_UMTS)
