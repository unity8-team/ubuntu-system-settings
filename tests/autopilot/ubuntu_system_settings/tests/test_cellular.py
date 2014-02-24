# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2014 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

from time import sleep

from autopilot.introspection.dbus import StateNotFoundError
from autopilot.matchers import Eventually
from testtools.matchers import Equals, NotEquals, raises
from unittest import expectedFailure

from ubuntu_system_settings.tests import UbuntuSystemSettingsOfonoTestCase
from ubuntu_system_settings.utils.i18n import ugettext as _

from ubuntuuitoolkit import emulators as toolkit_emulators

class CellularTestCase(UbuntuSystemSettingsOfonoTestCase):
    """ Tests for cellular Page """

    def navigate_to_manual(self):
        selector = self.cellular_page.select_single(
                toolkit_emulators.ItemSelector,
                objectName="autoChooseCarrierSelector")
        manual = selector.select_single('Label', text=_("Manually"))
        self.pointer.click_object(manual)
        choosecarrier = self.cellular_page.select_single(
                objectName="chooseCarrier")
        self.pointer.click_object(choosecarrier)
        self.assertThat(self.choose_page.title, Equals(_("Carrier")))

    def test_cellular_page(self):
        """ Checks whether Cellular page is available """
        self.assertThat(self.cellular_page, NotEquals(None))
        self.assertThat(self.cellular_page.title, Equals(_('Cellular')))

    def test_current_network(self):
        """ Tests whether the current network is visible and selected """
        self.navigate_to_manual()
        carriers = self.choose_page.select_single(
                toolkit_emulators.ItemSelector,
                objectName="carrierSelector")
        # TODO: Once there is a proper ItemSelector emulator, get the items
        # from it and check 'fake.tel' is the selected one.
        manual = carriers.select_single('Label', text="fake.tel")
        self.assertThat(manual, NotEquals(None))
        self.assertThat(carriers.selectedIndex, Equals(0))

    def test_alt_network(self):
        """ Tests whether an alternative available network is displayed """
        self.navigate_to_manual()
        carriers = self.choose_page.select_single(
                toolkit_emulators.ItemSelector,
                objectName="carrierSelector")
        manual = carriers.select_single('Label', text="my.cool.telco")
        self.assertThat(manual, NotEquals(None))

    def test_no_forbidden_network(self):
        """ Ensures that a forbidden network is not shown """
        self.navigate_to_manual()
        carriers = self.choose_page.select_single(
                toolkit_emulators.ItemSelector,
                objectName="carrierSelector")
        self.assertThat(lambda: carriers.select_single('Label', text="my.bad.telco"),
            raises(StateNotFoundError))
