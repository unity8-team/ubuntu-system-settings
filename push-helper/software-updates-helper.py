#!/usr/bin/python3
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

import json
import sys
import time
import gettext

if len(sys.argv) != 3:
    print("File in and out expected via argv", file=sys.stderr)
    sys.exit(1)

f1, f2 = sys.argv[1:3]
_ = gettext.translation("ubuntu-system-settings").gettext

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
# When the click server starts sending notifications of packages a user can
# update, you should probably describe that payload here. What to do with it
# is described in some detail in the wiki page above.
#
#
# Once you've downloaded things and need to actually notify the user, you'd
# send a notification as below, over dbus to Post. That's the third payload
# this script will be called with; for that case you'd just pass the payload
# through.
#
# For cases when you don't want to notify the user (yet), the correct
# output (to be written to a file whose name is f2) is “{}”, i.e. an
# empty json object.
#
# For now, this script assumes everything that comes in is a valid
# broadcast notification, and notifies the user directly:


icon = "/usr/share/ubuntu/settings/system/icons/settings-system-update.svg"
obj = {
    "notification": {
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
            "popup": True,
        },
    },
}

json.dump(obj, open(f2, "w"))
