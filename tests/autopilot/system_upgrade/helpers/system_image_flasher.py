# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2014 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

import subprocess
import tempfile
import time
import os


class DeviceImageFlash(object):

    def setup_device(self, ppa=None, package_list=None, **kwargs):
        self.flash_device(**kwargs)
        self.wait_for_device()
        self.set_device_read_write()
        self.reboot_device()
        self.disable_intro_screen()
        self.reboot_device()

        if ppa is not None:
            self.add_apt_repository(ppa)

        self.apt_get_update()

        if package_list is not None:
            self.apt_get_install(package_list)

    def str_to_lst(self, string):
        result = []
        for char in string.split():
            result.append(char)

        return result

    def run_command_on_host(self, command, str_to_list=True):
        if str_to_list is True:
            command = self.str_to_lst(command)

        subprocess.call(command)

    def run_autopilot_test_on_target(self, test_suite, user='phablet'):
        test_command = \
            "autopilot run -f text -o /tmp/upgrade.log -v {}".format(
                test_suite)
        command = 'adb shell sudo -iu {} bash -ic "{}"'.format(
            user, test_command)

        subprocess.call(
            command,
            shell=True,
        )

        subprocess.call('adb pull /tmp/upgrade.log /tmp', shell=True)

    def flash_device(
            self,
            channel='ubuntu-touch/trusty-proposed',
            revision_number=None,
            bootstrap=False
            ):

        command = []
        command.append('phablet-flash')
        command.append('ubuntu-system')
        command.append('--channel')
        command.append(channel)
        if revision_number is not None:
            command.append('--revision')
            command.append(revision_number)
        if bootstrap is True:
            command.append('--bootstrap')

        self.run_command_on_host(command, str_to_list=False)

    def run_command_on_target(self, command, user=None, shell=False):
        if user == 'phablet':
            command = 'adb shell sudo -iu {} bash -ic "{}"'.format(
                user, command
            )
            self.run_command_on_host(command)
        elif user is None:
            command = 'adb shell {}'.format(command)
            self.run_command_on_host(command)

    def wait_for_device(self, timeout=60):
        command = 'adb wait-for-devices'
        self.run_command_on_host(command)
        time.sleep(timeout)
        self.run_command_on_host(command)

    def set_device_read_write(self):
        command = 'touch /userdata/.writable_image'
        self.run_command_on_target(command)

    def reboot_device(self):
        self.run_command_on_target('reboot')
        time.sleep(10)
        self.wait_for_device()

    def disable_intro_screen(self):
        self.run_command_on_target(
            'dbus-send --system --print-reply '
            '--dest=org.freedesktop.Accounts '
            '/org/freedesktop/Accounts/User32011 '
            'org.freedesktop.DBus.Properties.Set '
            'string:com.canonical.unity.AccountsService '
            'string:demo-edges variant:boolean:false')
        self.run_command_on_target('touch /.intro_off')

    def setup_network(self):
        self.run_command_on_host('phablet-network')

    def add_apt_repository(self, ppa):
        command = 'add-apt-repository -y {}'.format(ppa)
        self.run_command_on_target(command)

    def apt_get_update(self):
        self.run_command_on_target('apt-get update')

    def apt_get_install(self, packages):
        command = 'apt-get -y -f --force-yes install {}'.format(packages)
        self.run_command_on_target(command)
