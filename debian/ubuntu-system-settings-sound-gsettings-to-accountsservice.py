#!/usr/bin/python3
# -*- coding: utf-8 -*-
# Copyright (C) 2013 Canonical
#
# Authors:
#  Iain Lane <iain.lane@canonical.com>
#
# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation; version 3.
#
# This program is distributed in the hope that it will be useful, but WITHOUTa
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License along with
# this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

# Migrate ubuntu-system-settings sound panel settings from GSettings to
# AccountsService

from __future__ import print_function

import dbus
import os
import sys

from gi.repository import Gio

system_bus = dbus.SystemBus()
manager_proxy = system_bus.get_object('org.freedesktop.Accounts',
                                      '/org/freedesktop/Accounts')
object_path = manager_proxy.FindUserById(
    os.getuid(), dbus_interface='org.freedesktop.Accounts'
)
user_proxy = system_bus.get_object('org.freedesktop.Accounts', object_path)

gsettings = Gio.Settings(schema="com.ubuntu.touch.sound")


def get_string(key):
    return gsettings.get_string(key)


def get_bool(key):
    return gsettings.get_boolean(key)


def set_as_setting(interface, setting, value):
    try:
        user_proxy.Set(interface, setting, value,
                       dbus_interface=dbus.PROPERTIES_IFACE)
    except dbus.exceptions.DBusException as e:
        print("Couldn't update %s: %s" % (setting, e), file=sys.stderr)

# (gsettings key, accountsservice key, function to retrieve from gsettings)
keys = [('silent-mode', 'SilentMode', get_bool),
        ('incoming-call-sound', 'IncomingCallSound', get_string),
        ('incoming-message-sound', 'IncomingMessageSound', get_string)]

for (g, a, f) in keys:
    set_as_setting('com.ubuntu.touch.AccountsService.Sound', a, f(g))
