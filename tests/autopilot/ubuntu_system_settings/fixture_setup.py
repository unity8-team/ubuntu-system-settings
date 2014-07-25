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

import fixtures

from ubuntu_system_settings import helpers


class RingtoneBackup(fixtures.Fixture):
    """Fixture to backup and restore ringtone."""

    def setUp(self):
        super(RingtoneBackup, self).setUp()
        self.addCleanup(self._restore_ringtone)
        self._backup_ringtone()

    def _backup_ringtone(self):
        self.ringtone_backup = helpers.get_current_ringtone_from_backend()

    def _restore_ringtone(self):
        iface = helpers.get_accounts_service_iface()

        iface.Set(
            'com.ubuntu.touch.AccountsService.Sound',
            'IncomingCallSound',
            self.ringtone_backup)
