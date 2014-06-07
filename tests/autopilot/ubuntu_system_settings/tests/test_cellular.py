# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2014 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.
from time import sleep

from autopilot.introspection.dbus import StateNotFoundError
from testtools.matchers import Equals, NotEquals, raises

from ubuntu_system_settings.tests import UbuntuSystemSettingsOfonoTestCase
from ubuntu_system_settings.utils.i18n import ugettext as _

from ubuntuuitoolkit import emulators as toolkit_emulators


class CellularTestCase(UbuntuSystemSettingsOfonoTestCase):
    """ Tests for cellular Page """

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

class TechnologyPreferenceTestCase(UbuntuSystemSettingsOfonoTestCase):

    def test_initial_selected(self):
        # assert that 'any' (from mock from setup) is selected
        data_type = self.system_settings.main_view.cellular_page.select_single(
            toolkit_emulators.ItemSelector,
            objectName="chooseDataTypeSelector"
        )
        # assert that 'any' is selected
        self.assertThat(data_type.selectedIndex, Equals(1))

    def test_setting_persistence(self):
        data_type = self.system_settings.main_view.cellular_page.select_single(
            toolkit_emulators.ItemSelector,
            objectName="chooseDataTypeSelector"
        )

        # click 2G
        type_2g = data_type.select_single('Label', text="2G")
        self.system_settings.main_view.pointer.click_object(type_2g)

        # go back
        self.system_settings.main_view.open_toolbar().click_back_button()
        # re-enter cellular settings
        cellular_item = self.system_settings.main_view.select_single(
            '*', objectName="entryComponent-cellular"
        )
        self.system_settings.main_view.pointer.click_object(cellular_item)

        # assert that "2G" is selected
        data_type = self.system_settings.main_view.cellular_page.select_single(
            toolkit_emulators.ItemSelector,
            objectName="chooseDataTypeSelector"
        )
        self.assertThat(data_type.selectedIndex, Equals(2))


    def test_available_preferences_are_enumerated(self):
        # see that mocked tech prefs
        pass

    def test_can_change_preferred_technology(self):
        # choose another technology (gsm)
        # assert that ofono.RadioSettings registers the change
        pass

    def test_no_technologies_available(self):
        # change dbusmock to have no technologies
        # assert that things does not break
        pass

    def test_turn_off_cellular_data(self):
        # turn off cellular data, leave preferred tech alone
        pass
