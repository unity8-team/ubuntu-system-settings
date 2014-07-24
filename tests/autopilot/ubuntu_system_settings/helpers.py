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
    uid = os.geteuid()
    bus = dbus.SystemBus()
    proxy = bus.get_object(
        'org.freedesktop.Accounts',
        '/org/freedesktop/Accounts/User{}'.format(uid))

    return dbus.Interface(proxy, 'org.freedesktop.DBus.Properties')


def get_current_ringtone_from_backend():
    accounts_iface = get_accounts_service_iface()

    return accounts_iface.Get(
        'com.ubuntu.touch.AccountsService.Sound',
        'IncomingCallSound')
