/*
 * Copyright (C) 2013-2016 Canonical Ltd
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
 * Alberto Mardegan <alberto.mardegan@canonical.com>
 */

import QtQuick 2.4
import SystemSettings 1.0
import SystemSettings.ListItems 1.0 as SettingsListItems
import Ubuntu.Components 1.3
import Ubuntu.Components.ListItems 1.3 as ListItems
import Ubuntu.SystemSettings.SecurityPrivacy 1.0

ItemPage {
    id: root
    title: i18n.tr("App permissions")
    flickable: scrollWidget

    onPushedOntoStack: {
        var service;
        if (pluginOptions && pluginOptions['service']) {
            service = pluginOptions['service'];
            for (var i = 0; i < appsModel.count; i++) {
                var item = appsModel.get(i)
                if (item.service === service) {
                    var model = trustStoreModelComponent.createObject(null, { serviceName: item.trustStoreService })
                    pageStack.addPageToNextColumn(
                        root, Qt.resolvedUrl("AppAccessControl.qml"), {
                        "title": i18n.tr(item.name),
                        "caption": i18n.tr(item.caption),
                        "model": model,
                    });
                    return;
                }
            }
        }
    }

    Flickable {
        id: scrollWidget
        anchors.fill: parent
        contentHeight: contentItem.childrenRect.height
        boundsBehavior: (contentHeight > root.height) ?
                            Flickable.DragAndOvershootBounds :
                            Flickable.StopAtBounds
        /* Set the direction to workaround
           https://bugreports.qt-project.org/browse/QTBUG-31905 otherwise the UI
           might end up in a situation where scrolling doesn't work */
        flickableDirection: Flickable.VerticalFlick

        Column {
            anchors.left: parent.left
            anchors.right: parent.right

            ListItems.Caption {
                text: i18n.tr("Apps that you have granted access to:")
            }

            ListModel {
                id: appsModel
                ListElement {
                    name: QT_TR_NOOP("Camera")
                    caption: QT_TR_NOOP("Apps that have requested access to your camera")
                    trustStoreService: "CameraService"
                    service: "camera"
                }
                ListElement {
                    name: QT_TR_NOOP("Location")
                    caption: QT_TR_NOOP("Apps that have requested access to your location")
                    trustStoreService: "UbuntuLocationService"
                    service: "location"
                }
                ListElement {
                    name: QT_TR_NOOP("Microphone")
                    caption: QT_TR_NOOP("Apps that have requested access to your microphone")
                    trustStoreService: "PulseAudio"
                    service: "microphone"
                }
            }

            Repeater {
                model: appsModel

                SettingsListItems.SingleValueProgression {
                    text: i18n.tr(model.name)
                    enabled: trustStoreModel.count > 0
                    progressionVisible: enabled ? true : false
                    value: trustStoreModel.count > 0 ?
                        i18n.tr("%1/%2").arg(trustStoreModel.grantedCount).arg(trustStoreModel.count) :
                        i18n.tr("0")
                    onClicked: pageStack.addPageToNextColumn(root, Qt.resolvedUrl("AppAccessControl.qml"), {
                        "title": i18n.tr(model.name),
                        "caption": i18n.tr(model.caption),
                        "model": trustStoreModel,
                    })

                    resources: TrustStoreModel {
                        id: trustStoreModel
                        serviceName: model.trustStoreService
                    }
                }
            }

            ListItems.Caption {
                    text: i18n.tr("Apps may also request access to online accounts.")
            }

            SettingsListItems.SingleControl {
                Button {
                    text: i18n.tr("Online Accounts…")
                    width: parent.width - units.gu(4)
                    onClicked: {
                        var oaPlugin = pluginManager.getByName("online-accounts")
                        if (oaPlugin) {
                            var accountsPage = oaPlugin.pageComponent
                            if (accountsPage)
                                pageStack.addPageToNextColumn(root, accountsPage, {plugin: oaPlugin, pluginManager: pluginManager})
                            else
                                console.warn("online-accounts")
                        } else {
                            console.warn("online-accounts")
                        }
                    }
                }
                showDivider: false
            }
        }
    }

    Component {
        id: trustStoreModelComponent
        TrustStoreModel {}
    }

}
