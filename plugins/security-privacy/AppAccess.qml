/*
 * Copyright (C) 2013 Canonical Ltd
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
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
 * Sebastien Bacher <sebastien.bacher@canonical.com>
 *
 */

import QtQuick 2.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import Ubuntu.SystemSettings.SecurityPrivacy 1.0
import SystemSettings 1.0

ItemPage {
    id: root
    title: i18n.tr("Other app access")

    Column {
        anchors.left: parent.left
        anchors.right: parent.right

        ListItem.Caption {
            text: i18n.tr("Apps that you have granted and have requested access to:")
        }

        ListModel {
            id: appsModel
            ListElement {
                name: "Camera"
                caption: "Apps that have requested access to your camera"
                trustStoreService: "CameraService"
            }
            ListElement {
                name: "Mic"
                caption: "Apps that have requested access to your mic"
                trustStoreService: "PulseAudio"
            }
        }

        Repeater {
            model: appsModel

            ListItem.SingleValue {
                text: model.name
                enabled: trustStoreModel.count > 0
                value: trustStoreModel.count > 0 ?
                    i18n.tr("%1/%2").arg(trustStoreModel.grantedCount).arg(trustStoreModel.count) :
                    i18n.tr("0")
                onClicked: pageStack.push(Qt.resolvedUrl("AppAccessControl.qml"), {
                    "title": model.name,
                    "caption": model.caption,
                    "model": trustStoreModel,
                })

                TrustStoreModel {
                    id: trustStoreModel
                    serviceName: model.trustStoreService
                }
            }
        }
    }
}
