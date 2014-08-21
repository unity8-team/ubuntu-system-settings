# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2014 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

from gi.repository import Gio
from time import sleep
from testtools.matchers import Equals, NotEquals

from ubuntu_system_settings.tests import SecurityBaseTestCase

from ubuntu_system_settings.utils.i18n import ugettext as _
from ubuntuuitoolkit import emulators as toolkit_emulators


class SecurityTestCase(SecurityBaseTestCase):
    """ Tests for Security Page """

    def setUp(self):
        super(SecurityTestCase, self).setUp()
        prps = self.system_settings.main_view.security_page.get_properties()
        self.use_powerd = prps['usePowerd']
        if self.use_powerd:
            gsettings = Gio.Settings.new('com.ubuntu.touch.system')
            prev = gsettings.get_uint('activity-timeout')
            self.addCleanup(
                self.set_prev_activity_timeout, gsettings, prev)
        else:
            gsettings = Gio.Settings.new('org.gnome.desktop.session')
            prev = gsettings.get_uint('idle-delay')
            self.addCleanup(
                self.set_prev_idle_delay, gsettings, prev)

    def set_prev_idle_delay(self, gsettings, prev):
        gsettings.set_uint('idle-delay', prev)
        # wait for gsettings
        sleep(0.2)

    def set_prev_activity_timeout(self, gsettings, prev):
        gsettings.set_uint('activity-timeout', prev)
        # wait for gsettings
        sleep(0.2)

    def _get_activity_timeout(self):
        if self.use_powerd:
            gsettings = Gio.Settings.new('com.ubuntu.touch.system')
            prev = gsettings.get_uint('activity-timeout')
            return prev
        else:
            gsettings = Gio.Settings.new('org.gnome.desktop.session')
            prev = gsettings.get_uint('idle-delay')
            return prev

    def _go_to_phone_lock(self):
        selector = self.security_page.select_single(
            objectName="lockingControl"
        )
        self.system_settings.main_view.scroll_to_and_click(selector)

    def _go_to_sleep_values(self):
        self._go_to_phone_lock()
        selector = self.system_settings.main_view.select_single(
            objectName="lockTimeout"
        )
        self.system_settings.main_view.scroll_to_and_click(selector)

    def _get_sleep_selector(self):
        self._go_to_sleep_values()
        sleep_values_page = self.system_settings.main_view.select_single(
            objectName='sleepValues')
        self.assertThat(
            sleep_values_page,
            NotEquals(None)
        )
        self._go_to_sleep_values()
        sleep_values_page = self.system_settings.main_view.select_single(
            objectName='sleepValues')
        return sleep_values_page.select_single(
            toolkit_emulators.ItemSelector,
            objectName="sleepSelector"
        )

    def test_security_page(self):
        """ Checks whether Security page is available """
        self.assertThat(
            self.system_settings.main_view.security_page,
            NotEquals(None)
        )
        self.assertThat(
            self.system_settings.main_view.security_page.title,
            Equals(_('Security & Privacy'))
        )

    def test_locking_control_value(self):
        actTimeout = self._get_activity_timeout()
        activityTimeout = self.security_page.select_single(
            objectName='lockingControl').value
        if actTimeout is 0:
            self.assertEquals(activityTimeout, ('Manually'))
        elif actTimeout is 60:
            self.assertEquals(
                activityTimeout,
                ('After {:d} minute').format(int(actTimeout/60)))
        else:
            self.assertEquals(
                activityTimeout,
                ('After {:d} minutes').format(int(actTimeout/60)))

    def test_phone_lock_page(self):
        self._go_to_phone_lock()
        phone_lock_page = self.system_settings.main_view.select_single(
            objectName='phoneLockingPage')
        self.assertThat(
            phone_lock_page,
            NotEquals(None)
        )
        self.assertThat(
            phone_lock_page.title,
            Equals(_('Phone locking'))
        )

    def test_phone_lock_value(self):
        self._go_to_phone_lock()
        phone_lock_page = self.system_settings.main_view.select_single(
            objectName='phoneLockingPage')
        actTimeout = self._get_activity_timeout()
        activityTimeout = phone_lock_page.select_single(
            objectName='lockTimeout').value
        if actTimeout is 0:
            self.assertEquals(activityTimeout, ('Never'))
        elif actTimeout is 60:
            self.assertEquals(
                activityTimeout,
                ('{:d} minute').format(int(actTimeout/60))
            )
        else:
            self.assertEquals(
                activityTimeout,
                ('{:d} minutes').format(int(actTimeout/60))
            )

    def test_idle_never_timeout(self):
        selector = self._get_sleep_selector()
        to_select = selector.select_single(
            'OptionSelectorDelegate', text='Never')
        self.system_settings.main_view.pointing_device.click_object(to_select)
        to_select.selected.wait_for(True)
        sleep(1)
        actTimeout = self._get_activity_timeout()
        self.assertEquals(actTimeout, 0)
        selected_delegate = selector.select_single(
            'OptionSelectorDelegate', selected=True)
        self.assertEquals(selected_delegate.text, 'Never')

    def test_idle_change_timeout(self):
        selector = self._get_sleep_selector()
        to_select = selector.select_single(
            'OptionSelectorDelegate', text='After 4 minutes')
        self.system_settings.main_view.pointing_device.click_object(to_select)
        to_select.selected.wait_for(True)
        sleep(1)
        actTimeout = self._get_activity_timeout()
        self.assertEquals(actTimeout, 240)
        selected_delegate = selector.select_single(
            'OptionSelectorDelegate', selected=True)
        self.assertEquals(selected_delegate.text, 'After 4 minutes')
