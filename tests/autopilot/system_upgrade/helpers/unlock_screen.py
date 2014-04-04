#!/usr/bin/env python

import logging
import sys

from unity8 import process_helpers as helpers

import dbus

logging.basicConfig(level=logging.INFO)


class UnlockScreen(object):

    def __init__(self):
        self.powerd = self._get_powerd_interface()

    def _get_powerd_interface(self):
        bus = dbus.SystemBus()
        return bus.get_object(
            'com.canonical.powerd', '/com/canonical/powerd'
        )

    def _powerd_request_state_active(self):
        logging.info('Setting system state "Active"')
        return self.powerd.requestSysState(
            'autopilot-lock', 1, dbus_interface='com.canonical.powerd'
        )

    def _powerd_release_state_active(self):
        logging.info('Releasing system state "Active"')
        self.powerd.clearSysState(
            self.active_cookie, dbus_interface='com.canonical.powerd'
        )

    def restart_with_testability(self):
        self.active_cookie = self._powerd_request_state_active()
        helpers.restart_unity_with_testability()
        self._powerd_release_state_active()

    def unlock_screen(self):
        self.restart_with_testability()
        helpers.unlock_unity()


def help():
    print("Usage:")
    print("Run the script without any argument to unlock with assertion.")


if len(sys.argv) >= 2 and sys.argv[1] == '-h':
    help()
else:
    unlocker = UnlockScreen()
    unlocker.unlock_screen()
