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
from testtools.matchers import Equals, raises, StartsWith
from unittest import skip

from ubuntu_system_settings.tests import (
    CellularBaseTestCase, RDO_IFACE,
    NETREG_IFACE, ACCOUNTS_PHONE_IFACE, CON_IFACE)

from ubuntu_system_settings.tests.connectivity import (
    SIM_IFACE as CTV_SIM_IFACE
)

DEV_IFACE = 'org.freedesktop.NetworkManager.Device'


class CellularTestCase(CellularBaseTestCase):

    def test_enable_data(self):
        self.ctv_private.Set(CON_IFACE, 'MobileDataEnabled', False)
        self.cellular_page.enable_data()
        self.assertThat(
            lambda: self.ctv_private.Get(CON_IFACE, 'MobileDataEnabled'),
            Eventually(Equals(True))
        )

    def test_disable_data(self):
        self.ctv_private.Set(CON_IFACE, 'MobileDataEnabled', True)
        self.cellular_page.disable_data()
        self.assertThat(
            lambda: self.ctv_private.Get(CON_IFACE, 'MobileDataEnabled'),
            Eventually(Equals(False))
        )

    def test_remote_manipulation_of_data(self):
        self.ctv_private.Set(CON_IFACE, 'MobileDataEnabled', True)
        self.assertThat(lambda: self.cellular_page.get_data(),
                        Eventually(Equals(True)))
        sleep(1)
        self.ctv_private.Set(CON_IFACE, 'MobileDataEnabled', False)
        sleep(1)
        self.assertThat(lambda: self.cellular_page.get_data(),
                        Eventually(Equals(False)))

    def test_enable_roaming(self):
        self.ctv_private.Set(CON_IFACE, 'MobileDataEnabled', True)
        self.ctv_sim0.Set(CTV_SIM_IFACE, 'DataRoamingEnabled', False)
        self.cellular_page.enable_roaming()
        self.assertThat(
            lambda: self.ctv_sim0.Get(CTV_SIM_IFACE, 'DataRoamingEnabled'),
            Eventually(Equals(True))
        )

    def test_disable_roaming(self):
        self.ctv_private.Set(CON_IFACE, 'MobileDataEnabled', True)
        self.ctv_sim0.Set(CTV_SIM_IFACE, 'DataRoamingEnabled', True)
        self.cellular_page.disable_roaming()
        self.assertThat(
            lambda: self.ctv_sim0.Get(CTV_SIM_IFACE, 'DataRoamingEnabled'),
            Eventually(Equals(False))
        )

    def test_connection_type(self):

        def get_pref():
            return self.modem_0.Get(RDO_IFACE, 'TechnologyPreference')

        for pref in ['lte', 'umts', 'gsm']:
            self.cellular_page.set_connection_type(pref)
            self.assertThat(get_pref, Eventually(Equals(pref)))

    def test_current_carrier(self):
        self.assertThat(lambda: self.modem_0.Get(NETREG_IFACE, 'Name'),
                        Eventually(Equals('fake.tel')))

    def test_change_carrier(self):
        self.cellular_page.change_carrier('my.cool.telco')

    def test_no_forbidden_network(self):
        """ Ensures that a forbidden network is not shown """
        self.assertThat(
            lambda: self.cellular_page.change_carrier('my.bad.telco'),
            raises(StateNotFoundError)
        )


class DualSimCellularTestCase(CellularBaseTestCase):

    use_sims = 2

    def test_data_off(self):
        self.ctv_private.Set(CON_IFACE, 'MobileDataEnabled', True)
        self.cellular_page.disable_data()
        self.assertThat(
            lambda: self.ctv_private.Get(CON_IFACE, 'MobileDataEnabled'),
            Eventually(Equals(False))
        )

    def test_sim1_online(self):
        self.ctv_private.Set(CON_IFACE,
                             'SimForMobileData',
                             dbus.ObjectPath("/"))
        self.cellular_page.select_sim_for_data('/ril_0')
        self.assertThat(
            lambda: self.ctv_private.Get(CON_IFACE, 'SimForMobileData'),
            Eventually(Equals(self.ctv_sim0.object_path))
        )

    def test_sim2_online(self):
        self.ctv_private.Set(CON_IFACE,
                             'SimForMobileData',
                             dbus.ObjectPath("/"))
        self.cellular_page.select_sim_for_data('/ril_1')
        self.assertThat(
            lambda: self.ctv_private.Get(CON_IFACE, 'SimForMobileData'),
            Eventually(Equals(self.ctv_sim1.object_path))
        )

    def test_connection_type_on_sim1(self):

        def get_pref():
            return self.modem_0.Get(RDO_IFACE, 'TechnologyPreference')

        sim = '/ril_0'
        stac = self.system_settings.main_view.scroll_to_and_click
        self.cellular_page.select_sim_for_data(sim)
        for pref in ['lte', 'umts', 'gsm']:
            self.cellular_page.set_connection_type(
                pref, sim=sim, scroll_to_and_click=stac)
            self.assertThat(get_pref, Eventually(Equals(pref)))

    def test_connection_type_on_sim2(self):

        def get_pref():
            return self.modem_1.Get(RDO_IFACE, 'TechnologyPreference')

        sim = '/ril_1'
        stac = self.system_settings.main_view.scroll_to_and_click
        self.cellular_page.select_sim_for_data(sim)
        for pref in ['gsm']:
            self.cellular_page.set_connection_type(
                pref, sim=sim, scroll_to_and_click=stac)
            self.assertThat(get_pref, Eventually(Equals(pref)))

    def test_current_carrier_sim1(self):
        self.assertThat(lambda: self.modem_0.Get(NETREG_IFACE, 'Name'),
                        Eventually(Equals('fake.tel')))

    def test_change_carrier_sim1(self):
        sim = '/ril_0'
        self.cellular_page.change_carrier('my.cool.telco', sim=sim)

    def test_current_carrier_sim2(self):
        self.assertThat(lambda: self.modem_1.Get(NETREG_IFACE, 'Name'),
                        Eventually(Equals('fake.tel')))

    def test_change_carrier_sim2(self):
        sim = '/ril_1'
        self.cellular_page.change_carrier('my.cool.telco', sim=sim)

    def test_change_sim1_name(self):

        sim = '/ril_0'
        try:
            old_name = self.obj_phone.GetSimNames()[sim]
        except:
            old_name = 'SIM 1'
        new_name = 'FOO BAR'
        self.cellular_page.set_name(sim, new_name)

        try:
            self.assertThat(
                lambda: self.obj_phone.GetSimNames()[sim],
                Eventually(Equals(new_name)))
        except Exception as e:
            raise e
        finally:
            self.cellular_page.set_name(sim, old_name)

    def test_change_sim2_name(self):
        sim = '/ril_1'

        try:
            old_name = self.obj_phone.GetSimNames()[sim]
        except:
            old_name = 'SIM 2'

        new_name = 'BAR BAZ'
        self.cellular_page.set_name(sim, new_name)

        try:
            self.assertThat(
                lambda: self.obj_phone.GetSimNames()[sim],
                Eventually(Equals(new_name)))
        except Exception as e:
            raise e
        finally:
            self.cellular_page.set_name(sim, old_name)

    def test_remote_manipulation_of_name(self):
        old_names = self.obj_phone.GetSimNames()
        sim = '/ril_0'
        name = 'BAS QUX'
        new_names = old_names
        new_names[sim] = name
        self.obj_phone.Set(ACCOUNTS_PHONE_IFACE, "SimNames", new_names)
        self.dbus_mock.EmitSignal(ACCOUNTS_PHONE_IFACE, "PropertyChanged",
                                  "sv", dbus.Array(["SimNames", new_names]))

        try:
            self.assertThat(
                lambda: self.cellular_page.get_name(sim),
                Eventually(StartsWith(name)))
        except Exception as e:
            raise e

    def test_roaming_switch(self):
        self.cellular_page.disable_data()
        # assert roaming_switch is disabled
        self.assertThat(
            lambda: self.cellular_page.enable_roaming(timeout=1),
            raises(AssertionError)
        )

    def test_allow_roaming_sim_1(self):
        self.ctv_private.Set(CON_IFACE, 'MobileDataEnabled', True)
        self.ctv_sim0.Set(CTV_SIM_IFACE, 'DataRoamingEnabled', False)
        self.ctv_private.Set(CON_IFACE,
                             'SimForMobileData',
                             dbus.ObjectPath("/"))
        self.cellular_page.select_sim_for_data('/ril_0')
        self.assertThat(
            lambda: self.ctv_private.Get(CON_IFACE, 'SimForMobileData'),
            Eventually(Equals(self.ctv_sim0.object_path))
        )
        self.cellular_page.enable_roaming()
        self.assertThat(
            lambda: self.ctv_sim0.Get(CTV_SIM_IFACE, 'DataRoamingEnabled'),
            Eventually(Equals(True))
        )

    def test_allow_roaming_sim_2(self):
        self.ctv_private.Set(CON_IFACE, 'MobileDataEnabled', True)
        self.ctv_sim1.Set(CTV_SIM_IFACE, 'DataRoamingEnabled', False)
        self.ctv_private.Set(CON_IFACE,
                             'SimForMobileData',
                             dbus.ObjectPath("/"))
        self.cellular_page.select_sim_for_data('/ril_1')
        self.assertThat(
            lambda: self.ctv_private.Get(CON_IFACE, 'SimForMobileData'),
            Eventually(Equals(self.ctv_sim1.object_path))
        )
        self.cellular_page.enable_roaming()
        self.assertThat(
            lambda: self.ctv_sim1.Get(CTV_SIM_IFACE, 'DataRoamingEnabled'),
            Eventually(Equals(True))
        )

    def test_changing_default_sim_for_calls(self):
        # click ask
        self.system_settings.main_view.scroll_to_and_click(
            self.get_default_sim_for_calls_selector('ask'))
        # click first sim
        self.system_settings.main_view.scroll_to_and_click(
            self.get_default_sim_for_calls_selector('/ril_0'))

        self.assertThat(
            lambda: self.obj_phone.GetDefaultSimForCalls(),
            Eventually(Equals('/ril_0')))

    def test_changing_default_sim_for_messages(self):
        # click ask
        self.system_settings.main_view.scroll_to_and_click(
            self.get_default_sim_for_messages_selector('ask'))
        # click second sim
        self.system_settings.main_view.scroll_to_and_click(
            self.get_default_sim_for_messages_selector('/ril_1'))

        self.assertThat(
            lambda: self.obj_phone.GetDefaultSimForMessages(),
            Eventually(Equals('/ril_1')))


class ApnTestCase(CellularBaseTestCase):

    @skip('Skip until we can get the CPO for ListItem trigger actions')
    def test_remove_apn(self):
        self.add_connection_context(self.modem_0, Type='mms', Name='Failed')
        contexts = self.modem_0.connMan.GetContexts()

        # Assert there's a Failed mms context
        self.assertEqual(1, len(contexts))
        self.assertEqual('/ril_0/context0', contexts[0][0])
        self.assertEqual('Failed', contexts[0][1]['Name'])

        # We delete the failed context
        self.cellular_page.delete_apn('Failed')

    def test_create_internet_apn(self):
        editor = self.cellular_page.open_apn_editor(None)
        editor.set_type('internet')
        editor.set_name('Ubuntu')
        editor.set_access_point_name('ubuntu.ap')
        editor.set_username('user')
        editor.set_password('pass')
        editor.save()

        # Wait for our new context to appear.
        self.assertThat(
            lambda: self.modem_0.connMan.GetContexts()[0][0],
            Eventually(Equals('/ril_0/context0'))
        )
        # Wait for our context to be renamed from the default ofono name.
        self.assertThat(
            lambda: self.modem_0.connMan.GetContexts()[0][1]['Name'],
            Eventually(Equals('Ubuntu'))
        )
        contexts = self.modem_0.connMan.GetContexts()
        new_ctx = contexts[0][1]
        self.assertEqual(1, len(contexts))
        self.assertEqual('internet', new_ctx['Type'])
        self.assertEqual('user', new_ctx['Username'])
        self.assertEqual('pass', new_ctx['Password'])

    def test_create_mms_apn(self):
        editor = self.cellular_page.open_apn_editor(None)
        editor.set_type('mms')
        editor.set_name('Ubuntu')
        editor.set_access_point_name('ubuntu.ap')
        editor.set_message_center('ubuntu.com')
        editor.set_message_proxy('ubuntu:8080')
        editor.set_username('user')
        editor.set_password('pass')
        editor.save()

        # Wait for our new context to appear.
        self.assertThat(
            lambda: self.modem_0.connMan.GetContexts()[0][0],
            Eventually(Equals('/ril_0/context0'))
        )
        # Wait for our context to be renamed from the default ofono name.
        self.assertThat(
            lambda: self.modem_0.connMan.GetContexts()[0][1]['Name'],
            Eventually(Equals('Ubuntu'))
        )
        contexts = self.modem_0.connMan.GetContexts()
        new_ctx = contexts[0][1]
        self.assertEqual(1, len(contexts))
        self.assertEqual('mms', new_ctx['Type'])
        self.assertEqual('ubuntu.ap', new_ctx['AccessPointName'])
        self.assertEqual('http://ubuntu.com', new_ctx['MessageCenter'])
        self.assertEqual('ubuntu:8080', new_ctx['MessageProxy'])
        self.assertEqual('user', new_ctx['Username'])
        self.assertEqual('pass', new_ctx['Password'])

    def test_create_mms_and_internet_apn(self):
        editor = self.cellular_page.open_apn_editor(None)
        editor.set_type('internet+mms')
        editor.set_name('Ubuntu')
        editor.set_access_point_name('ubuntu.ap')
        editor.set_message_center('ubuntu.com')
        editor.set_message_proxy('ubuntu:8080')
        editor.set_username('user')
        editor.set_password('pass')
        editor.save()

        # Wait for our new context to appear.
        self.assertThat(
            lambda: self.modem_0.connMan.GetContexts()[0][0],
            Eventually(Equals('/ril_0/context0'))
        )
        # Wait for our context to be renamed from the default ofono name.
        self.assertThat(
            lambda: self.modem_0.connMan.GetContexts()[0][1]['Name'],
            Eventually(Equals('Ubuntu'))
        )
        contexts = self.modem_0.connMan.GetContexts()
        new_ctx = contexts[0][1]
        self.assertEqual(1, len(contexts))
        self.assertEqual('internet', new_ctx['Type'])
        self.assertEqual('ubuntu.ap', new_ctx['AccessPointName'])
        self.assertEqual('http://ubuntu.com', new_ctx['MessageCenter'])
        self.assertEqual('ubuntu:8080', new_ctx['MessageProxy'])
        self.assertEqual('user', new_ctx['Username'])
        self.assertEqual('pass', new_ctx['Password'])

    def create_lte_apn(self):
        editor = self.cellular_page.open_apn_editor(None)
        editor.set_type('ia')
        editor.set_name('Ubuntu')
        editor.set_access_point_name('ubuntu.ap')
        editor.set_username('user')
        editor.set_password('pass')
        editor.save()

        # Wait for our new context to appear.
        self.assertThat(
            lambda: self.modem_0.connMan.GetContexts()[0][0],
            Eventually(Equals('/ril_0/context0'))
        )
        # Wait for our context to be renamed from the default ofono name.
        self.assertThat(
            lambda: self.modem_0.connMan.GetContexts()[0][1]['Name'],
            Eventually(Equals('Ubuntu'))
        )
        contexts = self.modem_0.connMan.GetContexts()
        new_ctx = contexts[0][1]
        self.assertEqual(1, len(contexts))
        self.assertEqual('ia', new_ctx['Type'])
        self.assertEqual('user', new_ctx['Username'])
        self.assertEqual('pass', new_ctx['Password'])

    def select_apn(self):
        self.add_connection_context(self.modem_0,
                                    Type='internet', Name='Provisioned')

        self.cellular_page.prefer_apn('Provisioned')

        # Assert that Preferred becomes true.
        self.assertThat(
            lambda: self.modem_0.connMan.GetContexts()[0][1]['Preferred'],
            Eventually(Equals(True))
        )

    def active_apn_is_automatically_preferred(self):
        self.add_connection_context(self.modem_0,
                                    Type='internet', Name='Provisioned',
                                    Active=True)
        self.cellular_page.open_apn_page(None)
        self.assertThat(
            lambda: self.modem_0.connMan.GetContexts()[0][1]['Preferred'],
            Eventually(Equals(True))
        )
