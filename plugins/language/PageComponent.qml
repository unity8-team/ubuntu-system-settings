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

    UbuntuLanguagePlugin {
        id: plugin
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
        id: keyboardLayouts

        KeyboardLayouts {}
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

            SettingsItemTitle {
                text: i18n.tr("Language")
            }

            SettingsListItems.Standard {
                id: base
                height: layout.height + divider.height
                objectName: "displayLanguage"
                property int currentLanguage: plugin.currentLanguage
                text: i18n.tr("Display language…")
                layout.subtitle.text: plugin.languageNames[plugin.currentLanguage]
                layout.subtitle.objectName: "currentLanguage"

                Icon {
                    source: "image://theme/language-chooser"
                    height: units.gu(2.5)
                    width: height
                    SlotsLayout.position: SlotsLayout.First
                }
                onClicked: PopupUtils.open(displayLanguage)
            }

            SettingsListItems.SingleValueProgression {
                text: i18n.tr("Keyboard layouts")
                value: plugin.keyboardLayoutsModel.subset.length == 1 ?
                       plugin.keyboardLayoutsModel.superset[plugin.keyboardLayoutsModel.subset[0]][0] :
                       plugin.keyboardLayoutsModel.subset.length
                onClicked: pageStack.push(keyboardLayouts)
            }

            SettingsItemTitle {
                text: i18n.tr("Correction")
            }

            SettingsListItems.SingleValueProgression {
                visible: showAllUI

                text: i18n.tr("Spell checking")
                value: plugin.spellCheckingModel.subset.length == 1 ?
                       plugin.spellCheckingModel.superset[plugin.spellCheckingModel.subset[0]][0] :
                       plugin.spellCheckingModel.subset.length

                onClicked: pageStack.push(spellChecking)
            }

            SettingsListItems.Standard {
                text: i18n.tr("Spell checking")

                Switch {
                    property bool serverChecked: settings.spellChecking
                    onServerCheckedChanged: checked = serverChecked
                    Component.onCompleted: checked = serverChecked
                    onTriggered: settings.spellChecking = checked
                }
            }

            SettingsListItems.Standard {
                text: i18n.tr("Auto correction")

                Switch {
                    property bool serverChecked: settings.autoCompletion
                    onServerCheckedChanged: checked = serverChecked
                    Component.onCompleted: checked = serverChecked
                    onTriggered: settings.autoCompletion = checked
                }
            }

            SettingsListItems.Standard {
                text: i18n.tr("Word suggestions")

                Switch {
                    property bool serverChecked: settings.predictiveText
                    onServerCheckedChanged: checked = serverChecked
                    Component.onCompleted: checked = serverChecked
                    onTriggered: settings.predictiveText = checked
                }
            }

            SettingsListItems.Standard {
                text: i18n.tr("Auto capitalization")
                layout.summary.text: i18n.tr(
                    "Turns on Shift to capitalize the first letter of each sentence."
                )

                Switch {
                    property bool serverChecked: settings.autoCapitalization
                    onServerCheckedChanged: checked = serverChecked
                    Component.onCompleted: checked = serverChecked
                    onTriggered: settings.autoCapitalization = checked
                }
            }

            SettingsListItems.Standard {
                text: i18n.tr("Auto punctuation")

                /* TODO: update the string to mention quotes/brackets once the osk does that */
                layout.summary.text: i18n.tr("Inserts a period when you tap Space twice.")

                Switch {
                    property bool serverChecked: settings.doubleSpaceFullStop
                    onServerCheckedChanged: checked = serverChecked
                    Component.onCompleted: checked = serverChecked
                    onTriggered: settings.doubleSpaceFullStop = checked
                }
            }

            SettingsListItems.Standard {
                text: i18n.tr("Keyboard sound")

                Switch {
                    property bool serverChecked: settings.keyPressFeedback
                    onServerCheckedChanged: checked = serverChecked
                    Component.onCompleted: checked = serverChecked
                    onTriggered: settings.keyPressFeedback = checked
                }
            }

            SettingsListItems.Standard {
                text: i18n.tr("Keyboard vibration")

                Switch {
                    property bool serverChecked: settings.keyPressHapticFeedback
                    onServerCheckedChanged: checked = serverChecked
                    Component.onCompleted: checked = serverChecked
                    onTriggered: settings.keyPressHapticFeedback = checked
                }
            }
        }
    }
}
