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
import GSettings 1.0
import SystemSettings 1.0
import Ubuntu.Components 0.1
import Ubuntu.Components.Popups 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
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
                i18n.domain = i18n.domain
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
                component: Label {
                    property int currentLanguage: plugin.currentLanguage
                    objectName: "currentLanguage"
                    text: plugin.languageNames[plugin.currentLanguage]
                    elide: Text.ElideRight
                    opacity: enabled ? 1.0 : 0.5
                }

                onClicked: PopupUtils.open(displayLanguage)
            }

            ListItem.Divider {
            }

            ListItem.SingleValue {
                text: i18n.tr("Keyboard layouts")
                value: plugin.keyboardLayoutsModel.subset.length == 1 ?
                       plugin.keyboardLayoutsModel.superset[plugin.keyboardLayoutsModel.subset[0]][0] :
                       plugin.keyboardLayoutsModel.subset.length
                progression: true

                onClicked: pageStack.push(keyboardLayouts)
            }

            ListItem.Divider {
            }

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
                    checked: settings.spellChecking

                    onClicked: settings.spellChecking = checked
                }
            }

            ListItem.Standard {
                text: i18n.tr("Auto correction")

                control: Switch {
                    checked: settings.autoCompletion

                    onClicked: settings.autoCompletion = checked
                }
            }

            ListItem.Standard {
                text: i18n.tr("Word suggestions")

                control: Switch {
                    checked: settings.predictiveText

                    onClicked: settings.predictiveText = checked
                }
            }

            ListItem.Divider {
            }

            ListItem.Standard {
                text: i18n.tr("Auto capitalization")

                control: Switch {
                    checked: settings.autoCapitalization

                    onClicked: settings.autoCapitalization = checked
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
                    checked: settings.doubleSpaceFullStop

                    onClicked: settings.doubleSpaceFullStop = checked
                }
            }

            ListItem.Caption {
                text: i18n.tr("Adds a period, and any missing quotes or brackets, when you tap Space twice.")
            }

            ListItem.ThinDivider {
            }

            ListItem.Standard {
                text: i18n.tr("Keyboard sound")

                control: Switch {
                    checked: settings.keyPressFeedback

                    onClicked: settings.keyPressFeedback = checked
                }
            }

            ListItem.Standard {
                text: i18n.tr("Keyboard vibration")

                control: Switch {
                    checked: settings.keyPressHapticFeedback

                    onClicked: settings.keyPressHapticFeedback = checked
                }
            }
        }
    }
}
