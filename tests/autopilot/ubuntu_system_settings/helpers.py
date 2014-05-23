# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
#
# Copyright (C) 2014 Canonical Ltd.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation; version 3.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.


from gi.repository import GLib, Gio


def get_current_ringtone_from_backend():
    """Returns the URI of the current selected tone from
    AccountsService.
    """
    uid = 1000
    bus = Gio.bus_get_sync(Gio.BusType.SYSTEM)
    result = bus.call_sync(
        "org.freedesktop.Accounts",
        ("/org/freedesktop/Accounts/User{}".format(uid)),
        "org.freedesktop.DBus.Properties", "Get",
        GLib.Variant("(ss)",
                    ("com.ubuntu.touch.AccountsService.Sound",
                        "IncomingCallSound")),
        GLib.VariantType.new("(v)"), Gio.DBusCallFlags.NONE, -1
    )

    return result.get_child_value(0).get_variant().get_string()
