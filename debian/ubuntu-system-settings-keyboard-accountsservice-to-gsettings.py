#!/usr/bin/python3
# -*- coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
#
# Copyright (C) 2016 Canonical
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
# this program.  If not, see <http://www.gnu.org/licenses/>.


# Copy ubuntu-system-settings keyboard panel settings from AccountsService to
# GSettings.  This might seem backwards (AS has the more modern location),
# and early on in Ubuntu Touch days, we did indeed only store the values in
# AccountsService.  But once we started using indicator-keyboard, which read
# from GSettings, it was easier to copy the values to both places rather than
# update the indicator and all other legacy consumers of the GSettings key in
# Ubuntu.  So we do a one-time sync here if we have values in AccountsService
# but not GSettings.  Along with a change to our code to write to both places
# when the user adjust settings.


import dbus
import os
import sys

from gi.repository import Gio
from gi.repository import GLib


system_bus = dbus.SystemBus()
manager_proxy = system_bus.get_object('org.freedesktop.Accounts',
                                      '/org/freedesktop/Accounts')
object_path = manager_proxy.FindUserById(
    os.getuid(), dbus_interface='org.freedesktop.Accounts'
)
user_proxy = system_bus.get_object('org.freedesktop.Accounts', object_path)

gsettings = Gio.Settings(schema="org.gnome.desktop.input-sources")


def get_accountsservice():
    try:
        return user_proxy.Get("org.freedesktop.Accounts.User", "InputSources",
                              dbus_interface=dbus.PROPERTIES_IFACE)
    except dbus.exceptions.DBusException as e:
        print("Couldn't get InputSources: %s" % (e), file=sys.stderr)
        return []


def has_gsettings():
    return bool(gsettings.get_value("sources"))


def set_gsettings(as_value):
    # AS stores a list of maps, GSettings stores a list of tuples.
    builder = GLib.VariantBuilder.new(GLib.VariantType.new("a(ss)"))
    for as_map in as_value:
        for as_key in as_map:
            gkey = GLib.Variant.new_string(as_key)
            gvalue = GLib.Variant.new_string(as_map[as_key])
            gtuple = GLib.Variant.new_tuple(gkey, gvalue)
            builder.add_value(gtuple)
    gsettings.set_value("sources", builder.end())


as_value = get_accountsservice()
if as_value and not has_gsettings():
    set_gsettings(as_value)
