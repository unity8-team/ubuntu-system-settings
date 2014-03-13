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

APP_PATH = '/usr/bin/system-settings'
DESKTOP_FILE = '/usr/share/applications/ubuntu-system-settings.desktop'

from autopilot import platform

from ubuntuuitoolkit import emulators as toolkit_emulators

def launch_system_settings(testobj, panel=None, emulator_base=None):
    """ Launch system settings application"""
    params = [APP_PATH]
    if (platform.model() != 'Desktop'):
        params.append('--desktop_file_hint={}'.format(DESKTOP_FILE))

    # Launch to a specific panel
    if panel is not None:
        params.append(panel)

    app = testobj.launch_test_application(
        *params,
        app_type='qt',
        emulator_base=toolkit_emulators.UbuntuUIToolkitEmulatorBase)

    return app
