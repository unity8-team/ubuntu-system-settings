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

import QtQuick 2.0
import SystemSettings 1.0
import Ubuntu.Components 0.1
import Ubuntu.Components.Popups 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import Ubuntu.SystemSettings.LanguagePlugin 1.0

ItemPage {
    id: root

    title: i18n.tr("Language & Text")

    UbuntuLanguagePlugin {
        id: plugin
    }

    DisplayLanguage {
        id: displayLanguage
    }

    Flickable {
        anchors.fill: parent
        contentHeight: contentItem.childrenRect.height
        boundsBehavior: contentHeight > root.height ? Flickable.DragAndOvershootBounds : Flickable.StopAtBounds

        Column {
            anchors.fill: parent

            ListItem.SingleValue {
                icon: "/usr/share/icons/ubuntu-mobile/actions/scalable/language-chooser.svg"
                text: i18n.tr("Display language")
                value: plugin.languages[plugin.currentLanguage]
                progression: true

                onClicked: PopupUtils.open(displayLanguage)
            }

            ListItem.Divider {}

            ListItem.SingleValue {
                text: i18n.tr("Keyboard layouts")
                value: plugin.pluginsModel.subset.length == 1 ?
                       plugin.pluginsModel.superset[plugin.pluginsModel.subset[0]] :
                       plugin.pluginsModel.subset.length
                progression: true

                onClicked: pageStack.push(Qt.resolvedUrl("KeyboardLayouts.qml"))
            }

            ListItem.Divider {}

            ListItem.SingleValue {
                text: i18n.tr("Spell checking")
                /* TODO: Get spell checking setting */
                value: "U.K. English"
                progression: true

                onClicked: pageStack.push(Qt.resolvedUrl("SpellChecking.qml"))
            }

            ListItem.Standard {
                text: i18n.tr("Auto correction")

                control: Switch {
                    checked: plugin.autoCorrection

                    onClicked: plugin.autoCorrection = checked
                }
            }

            ListItem.Standard {
                text: i18n.tr("Auto completion")

                control: Switch {
                    checked: plugin.autoCompletion

                    onClicked: plugin.autoCompletion = checked
                }
            }

            ListItem.Divider {}

            ListItem.Standard {
                text: i18n.tr("Auto capitalization")

                control: Switch {
                    checked: plugin.autoCapitalization

                    onClicked: plugin.autoCapitalization = checked
                }
            }

            ListItem.Caption {
                text: i18n.tr("Turns on Shift to capitalize the first letter of each sentence.")
            }

            ListItem.Divider {}

            ListItem.Standard {
                text: i18n.tr("Auto punctuation")

                control: Switch {
                    enabled: false
                    checked: plugin.autoPunctuation

                    onClicked: plugin.autoPunctuation = checked
                }
            }

            ListItem.Caption {
                text: i18n.tr("Adds a period, and any missing quotes or brackets, when you tap Space twice.")
            }
        }
    }
}
