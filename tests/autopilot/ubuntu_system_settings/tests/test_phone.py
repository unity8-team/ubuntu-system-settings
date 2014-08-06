# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2014 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

from __future__ import absolute_import

from ubuntu_system_settings.tests import PhoneOfonoBaseTestCase


class PhoneTestCase(PhoneOfonoBaseTestCase):
    """Tests for Phone Page"""

    def test_call_fwd(self):
        self.phone_page.go_to_call_forwarding()
