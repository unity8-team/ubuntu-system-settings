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
    """ Launch system settings application

        :param testobj: An AutopilotTestCase object, needed to call
        testobj.launch_test_application()

        :param panel: Launch to a specific panel. Default None.

        :param emulator_base: emulator_base for launch_test_application.

        :returns: A proxy object that represents the application. Introspection
        data is retrievable via this object.

    """
    params = [APP_PATH]
    if (platform.model() != 'Desktop'):
        params.append('--desktop_file_hint={}'.format(DESKTOP_FILE))

    # Launch to a specific panel
    if panel is not None:
        params.append(panel)

    # Default emulator base
    emulator = toolkit_emulators.UbuntuUIToolkitEmulatorBase
    if (emulator_base):
        # Use this instead
        emulator = emulator_base

    app = testobj.launch_test_application(
        *params,
        app_type='qt',
        emulator_base=emulator)

    return app
