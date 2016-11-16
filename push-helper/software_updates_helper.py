#!/usr/bin/python3
# -*- coding: utf-8 -*-
#
# Software Updates Push Notifications helper.
#
# This helper is called with one of three things:
# a) regular push messages about updated click packages¹
# b) broadcast messages about system updates
# c) notifications you send yourself over dbus to actually notify the user that
#    an update is ready to install².
#
# Figuring out which of those is the case is also this helper's job.
#
# notes:
# 1. nobody is sending those at the time of writing.
# 2. yes, this is rather convoluted. Most push helpers don't have to deal with
#    this stuff.

import os
import json
import sys
import time
import gettext
import logging
import logging.handlers
from xdg.BaseDirectory import save_cache_path

_ = gettext.translation('ubuntu-system-settings', fallback=True).gettext

SYS_UPDATE = "system-image-update"


class SystemImage:
    def __init__(self):
        self.loop = None
        self.sysimg = None
        self.postal = None
        self.notify = False

    def setup(self):
        import dbus
        from dbus.mainloop.glib import DBusGMainLoop
        from gi.repository import GLib

        gloop = DBusGMainLoop()
        sys_bus = dbus.SystemBus(mainloop=gloop)
        ses_bus = dbus.SessionBus(mainloop=gloop)
        self.loop = GLib.MainLoop()
        self.sysimg = dbus.Interface(
            sys_bus.get_object("com.canonical.SystemImage", "/Service"),
            dbus_interface="com.canonical.SystemImage")
        self.postal = dbus.Interface(
            ses_bus.get_object("com.ubuntu.Postal", "/com/ubuntu/Postal/_"),
            dbus_interface="com.ubuntu.Postal")
        self.notify = False

    def quit(self):
        if self.notify:
            logging.debug("Notifying.")
            # remove any older notifications about this
            self.postal.ClearPersistent("_ubuntu-system-settings", SYS_UPDATE)
            # send ours. This will of course come back to this same script.
            self.postal.Post("_ubuntu-system-settings", json.dumps(SYS_UPDATE))
        self.loop.quit()

    def available_cb(self, available, downloading, *ignored):
        logging.debug("Available: %s; downloading: %s", available, downloading)
        if available:
            if downloading:
                # handled in the UpdateDownloaded or UpdateFailed handlers
                return
            # available and not downloading: auto downloads are turned
            # off; notify the user right now.
            self.notify = True
        else:
            # if not available, we were called spuriously. No notification.
            self.notify = False
        self.quit()

    def downloaded_cb(self):
        logging.debug("Downloaded.")
        self.notify = True
        self.quit()

    def failed_cb(self, *ignored):
        # give up
        logging.debug("Failed.")
        self.notify = False
        self.quit()

    def run(self):
        logging.debug("Checking for update.")
        self.sysimg.connect_to_signal("UpdateAvailableStatus",
                                      self.available_cb)
        self.sysimg.connect_to_signal("UpdateDownloaded", self.downloaded_cb)
        self.sysimg.connect_to_signal("UpdateFailed", self.failed_cb)
        self.sysimg.CheckForUpdate()
        self.loop.run()


def main():
    if len(sys.argv) != 3:
        print("File in and out expected via argv", file=sys.stderr)
        sys.exit(1)

    f1, f2 = sys.argv[1:3]

    # here you should look at the input (the contents of the file whose
    # name is in f1, which are guaranteed to be json). If it's a broadcast
    # it will be the most recent we've received, and will have passed a
    # minimum amount of sanity checking, but you can probably do more. As
    # per the design on https://wiki.ubuntu.com/SoftwareUpdates#Prompting
    # if things are set to auto-download you should go download them (in a
    # child process -- this helper process itself has 4 more seconds to
    # live).
    #
    # the broadcast payload will be a single json object looking like
    #   { image-channel/device-model": [build-number, channel-alias]}
    #
    # e.g.,
    #
    #   {"ubuntu-touch/utopic-proposed/hammerhead":[265,""]}
    #
    #
    # When the click server starts sending notifications of packages a user
    # can update, you should probably describe that payload here. What to
    # do with it is described in some detail in the wiki page above.
    #
    #
    # Once you've downloaded things and need to actually notify the user,
    # you'd send a notification as below, over dbus to Post. That's the
    # third payload this script will be called with; for that case you'd
    # just pass the payload through.
    #
    # For cases when you don't want to notify the user (yet), the correct
    # output (to be written to a file whose name is f2) is “{}”, i.e. an
    # empty json object.

    with open(f1) as f:
        arg = json.load(f)

    obj = {}
    if arg == "system-image-update":
        logging.debug("system-image-update; requesting regular notification.")
        icon = "/usr/share/ubuntu/settings/system/icons/" + \
               "settings-system-update.svg"
        obj = {
            "notification": {
                "tag": SYS_UPDATE,
                "emblem-counter": {
                    "count": 1,
                    "visible": True,
                },
                "vibrate": {
                    "pattern": [50, 150],
                    "repeat": 3,
                },
                "card": {
                    "summary": _("There's an updated system image."),
                    "body": _("Tap to open the system updater."),
                    "actions": ["settings:///system/system-update"],
                    "icon": icon,
                    "timestamp": int(time.time()),
                    "persist": True,
                },
            },
        }
    elif arg == "testing":
        # for tests
        logging.debug("Testing.")
        obj = {"testing": True}
    else:
        # assume it's a broadcast
        logging.debug("Broadcast; forking.")
        if os.fork() == 0:
            os.setsid()
            os.closerange(0, 3)
            logging.debug("Forked.")
            s = SystemImage()
            s.setup()
            s.run()
            return

    json.dump(obj, open(f2, "w"))


if __name__ == '__main__':
    logdir = save_cache_path("ubuntu-system-settings")
    logfile = os.path.join(logdir, "software_updates_helper.log")
    rothandler = logging.handlers.TimedRotatingFileHandler(logfile, when="D",
                                                           backupCount=10)
    logging.basicConfig(
        format="%(asctime)s %(levelname)8s [%(process)04x] %(message)s",
        datefmt="%Y-%m-%dT%H:%M:%S",
        level=logging.DEBUG,
        handlers=(rothandler,))
    logging.debug("Starting.")
    try:
        main()
    except Exception:
        logging.exception("Died with exception:")
    else:
        logging.debug("Done.")
