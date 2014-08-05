# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2014 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

from __future__ import absolute_import

from time import sleep

from autopilot.matchers import Eventually
from gi.repository import Gio
from testtools.matchers import Contains, Equals

from ubuntu_system_settings.tests import PhoneOfonoBaseTestCase
from ubuntu_system_settings.utils.i18n import ugettext as _


class PhoneTestCase(PhoneOfonoBaseTestCase):
    """Tests for Phone Page"""

    def test_call_fwd(self):
        self.phone_page.open_call_forwarding()
