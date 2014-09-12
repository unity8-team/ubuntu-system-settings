/*
 * This file is part of system-settings
 *
 * Copyright (C) 2014 Canonical Ltd.
 *
 * Contact: Jonas G. Drange <jonas.drange@canonical.com>
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

import QtQuick 2.0
import MeeGo.QOfono 0.2
import Ubuntu.Components.ListItems 0.1 as ListItem

ListItem.SingleValue {

    property string path

    OfonoSimManager {
        id: sim
        modemPath: path
    }

    id: numberItem
    objectName: "numberItem"
    text: i18n.tr("Phone number")
    property string phoneNumber
    phoneNumber: sim.subscriberNumbers.length > 0 ?
        sim.subscriberNumbers[0] : ""
    value: phoneNumber
    visible: value
}
