/*
 * This file is part of system-settings
 *
 * Copyright (C) 2013 Canonical Ltd.
 *
 * Contact: William Hua <william.hua@canonical.com>
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
import QtSystemInfo 5.5
import GSettings 1.0
import SystemSettings 1.0
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.3
import Ubuntu.Components.ListItems 1.3 as ListItem
import Ubuntu.Settings.Menus 0.1 as Menus
import Ubuntu.SystemSettings.LanguagePlugin 1.0

ItemPage {
    id: root
    objectName: "languagePage"

    title: i18n.tr("Language & Text")

    InputDeviceManager {
        id: keyboardsModel
        filter: InputInfo.Keyboard
    }

    property bool externalKeyboardPresent: keyboardsModel.count > 0

    UbuntuLanguagePlugin {
        id: plugin
    }

    OnScreenKeyboardPlugin {
        id: oskPlugin
    }

    Component {
        id: displayLanguage

        DisplayLanguage {
            onLanguageChanged: {
                PopupUtils.open(rebootNecessaryNotification, root, {
                    revertTo: oldLanguage
                })
            }
        }
    }

    Component {
        id: spellChecking

        SpellChecking {}
    }

    Component {
        id: rebootNecessaryNotification

        RebootNecessary {

            onReboot: {
                plugin.reboot();
            }
            onRevert: {
                plugin.currentLanguage = to;
                i18n.language = plugin.languageCodes[to]
            }
        }
    }

    GSettings {
        id: settings

        schema.id: "com.canonical.keyboard.maliit"
    }

    Flickable {
        anchors.fill: parent
        contentHeight: contentItem.childrenRect.height
        boundsBehavior: contentHeight > root.height ?
                        Flickable.DragAndOvershootBounds :
                        Flickable.StopAtBounds
        /* Set the direction to workaround https://bugreports.qt-project.org/browse/QTBUG-31905
           otherwise the UI might end up in a situation where scrolling doesn't work */
        flickableDirection: Flickable.VerticalFlick

        Column {
            anchors.left: parent.left
            anchors.right: parent.right

            Menus.StandardMenu {
                iconSource: "image://theme/language-chooser"
                text: i18n.tr("Display language…")
                objectName: "displayLanguage"
                showDivider: false
                component: Label {
                    property int currentLanguage: plugin.currentLanguage
                    objectName: "currentLanguage"
                    text: plugin.languageNames[plugin.currentLanguage]
                    elide: Text.ElideRight
                    opacity: enabled ? 1.0 : 0.5
                }

                onClicked: PopupUtils.open(displayLanguage)
            }

            ListItem.Divider {}

            ListItem.SingleValue {
                text: externalKeyboardPresent ? i18n.tr("On-screen keyboard") :
                                                i18n.tr("Keyboard layouts")
                progression: true
                value: oskPlugin.keyboardLayoutsModel.subset.length == 1 ?
                       oskPlugin.keyboardLayoutsModel.superset[oskPlugin.keyboardLayoutsModel.subset[0]][0] :
                       oskPlugin.keyboardLayoutsModel.subset.length
                onClicked: pageStack.push(Qt.resolvedUrl("KeyboardLayouts.qml"), {
                    plugin: oskPlugin
                })
            }

            ListItem.Standard {
                text: i18n.tr("External keyboard")
                progression: true
                showDivider: false
                onClicked: pageStack.push(Qt.resolvedUrl("PageHardwareKeyboard.qml"))
                visible: externalKeyboardPresent
            }

            ListItem.Divider {}

            ListItem.SingleValue {
                visible: showAllUI

                text: i18n.tr("Spell checking")
                value: plugin.spellCheckingModel.subset.length == 1 ?
                       plugin.spellCheckingModel.superset[plugin.spellCheckingModel.subset[0]][0] :
                       plugin.spellCheckingModel.subset.length
                progression: true

                onClicked: pageStack.push(spellChecking)
            }

            ListItem.Standard {
                text: i18n.tr("Spell checking")

                control: Switch {
                    property bool serverChecked: settings.spellChecking
                    onServerCheckedChanged: checked = serverChecked
                    Component.onCompleted: checked = serverChecked
                    onTriggered: settings.spellChecking = checked
                }
            }

            ListItem.Standard {
                text: i18n.tr("Auto correction")

                control: Switch {
                    property bool serverChecked: settings.autoCompletion
                    onServerCheckedChanged: checked = serverChecked
                    Component.onCompleted: checked = serverChecked
                    onTriggered: settings.autoCompletion = checked
                }
            }

            ListItem.Standard {
                text: i18n.tr("Word suggestions")

                control: Switch {
                    property bool serverChecked: settings.predictiveText
                    onServerCheckedChanged: checked = serverChecked
                    Component.onCompleted: checked = serverChecked
                    onTriggered: settings.predictiveText = checked
                }
            }

            ListItem.Divider {
            }

            ListItem.Standard {
                text: i18n.tr("Auto capitalization")

                control: Switch {
                    property bool serverChecked: settings.autoCapitalization
                    onServerCheckedChanged: checked = serverChecked
                    Component.onCompleted: checked = serverChecked
                    onTriggered: settings.autoCapitalization = checked
                }
            }

            ListItem.Caption {
                text: i18n.tr("Turns on Shift to capitalize the first letter of each sentence.")
            }

            ListItem.ThinDivider {
            }

            ListItem.Standard {
                text: i18n.tr("Auto punctuation")

                control: Switch {
                    property bool serverChecked: settings.doubleSpaceFullStop
                    onServerCheckedChanged: checked = serverChecked
                    Component.onCompleted: checked = serverChecked
                    onTriggered: settings.doubleSpaceFullStop = checked
                }
            }

            ListItem.Caption {
                /* TODO: update the string to mention quotes/brackets once the osk does that */
                text: i18n.tr("Inserts a period when you tap Space twice.")
            }

            ListItem.ThinDivider {
            }

            ListItem.Standard {
                text: i18n.tr("Keyboard sound")

                control: Switch {
                    property bool serverChecked: settings.keyPressFeedback
                    onServerCheckedChanged: checked = serverChecked
                    Component.onCompleted: checked = serverChecked
                    onTriggered: settings.keyPressFeedback = checked
                }
            }

            ListItem.Standard {
                text: i18n.tr("Keyboard vibration")

                control: Switch {
                    property bool serverChecked: settings.keyPressHapticFeedback
                    onServerCheckedChanged: checked = serverChecked
                    Component.onCompleted: checked = serverChecked
                    onTriggered: settings.keyPressHapticFeedback = checked
                }
            }
        }
    }
}
