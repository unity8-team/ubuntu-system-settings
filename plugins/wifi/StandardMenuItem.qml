/*
 * Copyright 2013 Canonical Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors:
 *      Nick Dedekind <nick.dedekind@canonical.com>
 */

import QtQuick 2.0
import Ubuntu.Components 1.1

FramedMenuItem {
    id: menuItem

    property bool checkable: false
    property bool checked: false

    signal activate()

    onCheckedChanged: {
        // Can't rely on binding. Checked is assigned on click.
        if (checkable) {
            checkbox.checked = checked;
        }
    }

    onClicked: {
        if (checkable) {
            checkbox.clicked();
        } else {
            menuItem.activate();
        }
    }

    control: CheckBox {
        id: checkbox

        Component.onCompleted: {
            checked = menuItem.checked;
        }

        // FIXME : should use Checkbox.toggled signal
        // lp:~nick-dedekind/ubuntu-ui-toolkit/checkbox.toggled
        onClicked: {
            menuItem.activate();
        }

        visible: checkable
    }
}
