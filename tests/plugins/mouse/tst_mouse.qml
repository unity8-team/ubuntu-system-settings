/*
 * This file is part of system-settings
 *
 * Copyright (C) 2015 Canonical Ltd.
 *
 * Contact: Ken VanDine <ken.vandine@canonical.com>
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 3, as published
 * by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
import QtQuick 2.4
import QtTest 1.0

import SystemSettings 1.0
import Ubuntu.Components 1.2

import Source 1.0

TapArea {
    id: area
    height: 48
    width: height
    doubleTapSpeed: 200

    TestCase {
        name: "ItemTests"
        id: test1
        when: windowShown

        function test_double_click() {
            waitForRendering(area, 1000)
            mouseClick(area);
            wait(10);
            mouseClick(area);
            verify(area.lastStatus)

        }
    }

}
