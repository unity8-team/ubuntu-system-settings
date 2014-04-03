#!/usr/bin/env python

import logging
import sys

from unity8 import process_helpers as helpers

import dbus

logging.basicConfig(level=logging.INFO)


def unlock_screen():
    restart_with_testability()
    helpers.unlock_unity()


def restart_with_testability():
    powerd = _get_powerd_interface()
    powerd_cookie = _take_powerd_screen_lock(powerd)
    helpers.restart_unity_with_testability()
    _release_powerd_screen_lock(powerd, powerd_cookie)


def _get_powerd_interface():
    bus = dbus.SystemBus()
    return bus.get_object('com.canonical.powerd', '/com/canonical/powerd')


def _take_powerd_screen_lock(powerd):
    logging.info('Taking screen lock (#1235000).')
    return powerd.requestSysState(
        "autopilot-lock", 1, dbus_interface='com.canonical.powerd'
    )


def _release_powerd_screen_lock(powerd, powerd_cookie):
    logging.info('Releasing screen lock (#1235000)')
    powerd.clearSysState(
        powerd_cookie, dbus_interface='com.canonical.powerd'
    )


def help():
    print("Usage:")
    print("Run the script without any argument to unlock with assertion.")


if len(sys.argv) >= 2 and sys.argv[1] == '-h':
    help()
else:
    unlock_screen()
