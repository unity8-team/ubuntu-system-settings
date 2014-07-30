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

import os

import dbus


def get_accounts_service_iface():
    """Return the accountsservice dbus interface."""
    uid = os.geteuid()
    bus = dbus.SystemBus()
    proxy = bus.get_object(
        'org.freedesktop.Accounts',
        '/org/freedesktop/Accounts/User{}'.format(uid))

    return dbus.Interface(proxy, 'org.freedesktop.DBus.Properties')


def get_current_ringtone_uri_from_backend():
    """Return the URI of the current ringtone."""
    accounts_iface = get_accounts_service_iface()

    return accounts_iface.Get(
        'com.ubuntu.touch.AccountsService.Sound',
        'IncomingCallSound')


def get_current_ringtone_name_from_backend():
    """Return the name of the current ringtone stripped from URI."""
    ringtone_uri = get_current_ringtone_uri_from_backend()

    return ringtone_uri.lstrip(
        '/usr/share/sounds/ubuntu/ringtones/').rstrip('.ogg')


def get_non_selected_ringtone():
    """Return the name of a ringtone that is currently not selected."""
    ringtones = ['Supreme', 'Ubuntu']
    for ringtone in ringtones:
        if not get_current_ringtone_name_from_backend() == ringtone:
            return ringtone
