# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2013 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

import locale
import gettext

APP_NAME='ubuntu-system-settings'
LOCALE_DIR='/usr/share/locale/'

lc, encoding = locale.getdefaultlocale()

if not lc:
    lc = 'C'

language = gettext.translation(APP_NAME, LOCALE_DIR, languages=[lc], fallback=True)

# UTF-8
ugettext = language.ugettext
