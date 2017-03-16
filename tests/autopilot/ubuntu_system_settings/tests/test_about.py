# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2013 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

import subprocess

from autopilot.platform import model
from testtools import skipIf
from testtools.matchers import Equals

from ubuntu_system_settings.tests import (
    AboutBaseTestCase,
    AboutOfonoBaseTestCase,
    LicenseBaseTestCase
)

import dbus


class AboutTestCase(AboutBaseTestCase):

    """Tests for About this phone Page."""

    def _get_os_name(self):
        os_id = subprocess.check_output(
            ['lsb_release', '-is'], universal_newlines=True)
        os_release = subprocess.check_output(
            ['lsb_release', '-rs'], universal_newlines=True)

        return '{} {}'.format(os_id.strip(), os_release.strip())

    def _get_device_serial_number(self):
        try:
            return subprocess.check_output(
                ['getprop', 'ro.serialno'], universal_newlines=True).strip()
        except OSError:
            # getprop is only available on android hardware.
            return None

    def _get_device_manufacturer_and_model(self):
        if model() == 'Desktop':
            manufacturer = open(
                '/sys/devices/virtual/dmi/id/sys_vendor'
            ).read().strip()
            hw_model = open(
                '/sys/devices/virtual/dmi/id/product_name'
            ).read().strip()
        else:
            manufacturer = subprocess.check_output(
                ['getprop', 'ro.product.manufacturer'],
                universal_newlines=True
            ).strip()
            hw_model = subprocess.check_output(
                ['getprop', 'ro.product.model'],
                universal_newlines=True
            ).strip()

        return '{} {}'.format(manufacturer, hw_model)

    def test_device_with_serial_number_must_display_it(self):
        """Checks whether the UI is showing the correct serial number."""
        device_serial = self._get_device_serial_number()
        if not device_serial:
            self.skipTest('The device has no serial number.')
        else:
            self.assertTrue(self.about_page.is_serial_visible())
            displayed_serial = self.about_page.get_serial()
            self.assertThat(
                displayed_serial, Equals(device_serial))

    def test_device_without_serial_must_not_display_it(self):
        device_serial = self._get_device_serial_number()
        if device_serial:
            self.skipTest('The device has serial number.')
        else:
            self.assertFalse(self.about_page.is_serial_visible())

    def test_settings_show_correct_version_of_the_os(self):
        """Ensure the UI is showing the correct version of the OS."""
        device_os_version = self._get_os_name()
        displayed_os_info = self.about_page.get_os_information()
        # TODO: find a way to check the image build number as well
        # -- om26er 10-03-2014
        self.assertTrue(device_os_version in displayed_os_info)

    @skipIf(subprocess.call(
        ['which', 'getprop'], stdout=subprocess.PIPE) != 0,
        'program "getprop" not found'
    )
    def test_hardware_name(self):
        """Ensure the UI is showing the correct device name."""
        displayed_device_name = self.about_page.get_device_name()
        device_name_from_getprop = self._get_device_manufacturer_and_model()

        self.assertEquals(displayed_device_name, device_name_from_getprop)


class AboutOfonoTestCase(AboutOfonoBaseTestCase):
    def _get_imei_from_dbus(self):
        bus = self.get_dbus(system_bus=True)
        try:
            manager = dbus.Interface(
                bus.get_object('org.ofono', '/'), 'org.ofono.Manager'
            )
        except dbus.exceptions.DBusException:
            # oFono interface not found, probably its a desktop.
            return None

        modems = manager.GetModems()
        for path, properties in modems:
            return properties['Serial'] if 'Serial' in properties else None

    def test_device_with_imei_must_display_it(self):
        """Checks whether the UI is exposing the right IMEI."""
        device_imei = self._get_imei_from_dbus()
        if not device_imei:
            self.skipTest('The device has no imei.')
        else:
            self.assertTrue(self.about_page.is_imei_visible())
            displayed_imei = self.about_page.get_imei()
            self.assertThat(displayed_imei, Equals(device_imei))


class LicenseTestCase(LicenseBaseTestCase):

    """Tests for Licenses."""

    def test_open_licenses_page(self):
        """Check whether Storage page is available."""
        # FIXME this is not a good Autopilot tests. It would be better if it
        # opens one of the licenses and checks that it is visible.
        # --elopio - 2014-07-02
        self.assertThat(self.licenses_page.active, Equals(True))
