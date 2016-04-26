# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2014 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

from gi.repository import Gio, GLib
from time import sleep

from autopilot.introspection.dbus import StateNotFoundError
from autopilot.matchers import Eventually
from testtools.matchers import Equals, raises, StartsWith

from ubuntu_system_settings.tests import (
    CellularBaseTestCase, CONNMAN_IFACE, RDO_IFACE,
    NETREG_IFACE)


DEV_IFACE = 'org.freedesktop.NetworkManager.Device'


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
        self.cellular_page.disable_datas()
        self.assertThat(
            lambda: self.modem_0.Get(CONNMAN_IFACE, 'Powered'),
            Eventually(Equals(False))
        )
        self.assertThat(
            lambda: self.modem_1.Get(CONNMAN_IFACE, 'Powered'),
            Eventually(Equals(False))
        )

    def test_sim1_online(self):
        self.cellular_page.select_sim_for_data('/ril_0')
        self.assertThat(
            lambda: self.modem_0.Get(CONNMAN_IFACE, 'Powered'),
            Eventually(Equals(True))
        )
        self.assertThat(
            lambda: self.modem_1.Get(CONNMAN_IFACE, 'Powered'),
            Eventually(Equals(False))
        )

    def test_sim2_online(self):
        self.cellular_page.select_sim_for_data('/ril_1')
        self.assertThat(
            lambda: self.modem_0.Get(CONNMAN_IFACE, 'Powered'),
            Eventually(Equals(False))
        )
        self.assertThat(
            lambda: self.modem_1.Get(CONNMAN_IFACE, 'Powered'),
            Eventually(Equals(True))
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
        gsettings = Gio.Settings.new('com.ubuntu.phone')
        sim = '/ril_0'
        try:
            old_name = gsettings.get_value('sim-names')[sim]
        except:
            old_name = 'SIM 1'
        new_name = 'FOO BAR'
        self.cellular_page.set_name(sim, new_name)

        try:
            self.assertThat(
                lambda: gsettings.get_value('sim-names')[sim],
                Eventually(Equals(new_name)))
        except Exception as e:
            raise e
        finally:
            self.cellular_page.set_name(sim, old_name)
            # wait for gsettings
            sleep(1)

    def test_change_sim2_name(self):
        gsettings = Gio.Settings.new('com.ubuntu.phone')
        sim = '/ril_1'

        try:
            old_name = gsettings.get_value('sim-names')[sim]
        except:
            old_name = 'SIM 2'

        new_name = 'BAR BAZ'
        self.cellular_page.set_name(sim, new_name)

        try:
            self.assertThat(
                lambda: gsettings.get_value('sim-names')[sim],
                Eventually(Equals(new_name)))
        except Exception as e:
            raise e
        finally:
            self.cellular_page.set_name(sim, old_name)
            # wait for gsettings
            sleep(1)

    def test_remote_manipulation_of_name(self):
        gsettings = Gio.Settings.new('com.ubuntu.phone')
        old_names = gsettings.get_value('sim-names')
        sim = '/ril_0'
        name = 'BAS QUX'
        new_names = old_names.unpack()
        new_names[sim] = name
        new_names = GLib.Variant('a{ss}', new_names)
        gsettings.set_value('sim-names', new_names)
        try:
            self.assertThat(
                lambda: self.cellular_page.get_name(sim),
                Eventually(StartsWith(name)))
        except Exception as e:
            raise e
        finally:
            gsettings.set_value('sim-names', old_names)
            # wait for gsettings
            sleep(1)

    def test_roaming_switch(self):
        self.cellular_page.disable_datas()
        # assert roaming_switch is disabled
        self.assertThat(
            lambda: self.cellular_page.enable_roaming(timeout=1),
            raises(AssertionError)
        )

    def test_allow_roaming_sim_1(self):
        sim = '/ril_0'
        self.cellular_page.select_sim_for_data(sim)

        self.assertEqual(
            False, self.modem_0.Get(CONNMAN_IFACE, 'RoamingAllowed'))
        self.cellular_page.enable_roaming()
        self.assertThat(
            lambda: self.modem_0.Get(CONNMAN_IFACE, 'RoamingAllowed'),
            Eventually(Equals(True)))

    def test_allow_roaming_sim_2(self):
        sim = '/ril_1'
        self.cellular_page.select_sim_for_data(sim)

        self.assertEqual(
            False, self.modem_1.Get(CONNMAN_IFACE, 'RoamingAllowed'))
        self.cellular_page.enable_roaming()
        self.assertThat(
            lambda: self.modem_1.Get(CONNMAN_IFACE, 'RoamingAllowed'),
            Eventually(Equals(True)))

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

        self.assertThat(
            lambda: gsettings.get_value('default-sim-for-calls').get_string(),
            Eventually(Equals('/ril_0')))

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

        self.assertThat(
            lambda:
                gsettings.get_value('default-sim-for-messages').get_string(),
            Eventually(Equals('/ril_1')))


class ApnTestCase(CellularBaseTestCase):

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
