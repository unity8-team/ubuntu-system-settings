/*
 * This file is part of system-settings
 *
 * Copyright (C) 2015 Canonical Ltd.
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
import Ubuntu.Components 1.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import Ubuntu.Components.Themes.Ambiance 0.1
import MeeGo.QOfono 0.2
import "callForwardingUtils.js" as Utils

Column {
    id: root

    /**
     * Rule name.
     */
    property string ruleName

    /**
     * Represents the call forwarding rule on the client.
     */
    property alias clientRule: field.text

    /**
     * Holds a cached rule to use if user enables rule.
     */
    property var cachedRuleValue

    /**
     * The Ofono CallForwarding object.
     */
    property OfonoCallForwarding callForwarding

    /**
     * Enable or disable managing this forwarding rule.
     */
    property bool enabled: true

    property alias text: control.text

    /**
     * Signal emitted when user starts editing rule. The parent page will
     * handle all subsequent events (contacts import, set and cancel).
     */
    signal editing ()

    /**
     * Signal emitted when user stops editing a rule.
     */
    signal stoppedEditing ()

    /**
     * When called, this component will ask OfonoCallForwarding to update
     * the server property.
     */
    function setRule () {
        console.warn('setRule', clientRule);
        Utils.requestRule(clientRule);
    }

    /**
     * The parent page will receive the signals from OfonoCallForwarding and
     * subsequently call this function.
     *
     * @param {Boolean} success or not
     */
    function serverResponse (success) {
        if (success) {
            d._pending = false;
            d._editing = false;
            check.checked = clientRule !== "";
        } else {
            d._failed = true;
        }
    }

    /**
     * Cancels editing.
     */
    function cancel () {
        console.warn('cancelling...');
        d._editing = false;
        check.checked = false;
    }

    function useContact (contact) {

    }

    QtObject {
        id: d

        /**
         * Server is working.
         */
        property bool _pending: false

        /**
         * Server failed to change/fetch setting.
         */
        property bool _failed: false

        /**
         * If user checks box, we're editing the rule.
         */
        property bool _editing: false

        on_EditingChanged: {
            if (_editing) {
                root.editing();
                console.warn('firing editing')
            } else {
                console.warn('firing stoppedEditing')
                root.stoppedEditing();
            }
        }

        /**
         * Whether or not the forwarding rule is active.
         */
        property bool _active: typeof callForwarding[ruleName] !== "undefined"
                               ? callForwarding[ruleName] !== "" : false
    }

    states: [
        State {
            name: "failed"
            when: d._failed
            PropertyChanges { target: control; enabled: false }
            PropertyChanges { target: failed; visible: true }
        },
        State {
            name: "disabled"
            when: !enabled
            PropertyChanges { target: control; enabled: false }
        },
        State {
            name: "requesting"
            when: d._editing && d._pending
            PropertyChanges { target: control; control: activity }
            PropertyChanges { target: check; enabled: false; visible: false }
            PropertyChanges { target: current; enabled: false; visible: true }
        },
        State {
            name: "pending"
            when: d._pending
            PropertyChanges { target: control; control: activity }
            PropertyChanges { target: check; enabled: false; visible: false }
            PropertyChanges { target: current; enabled: false; visible: false }
        },
        State {
            name: "editing"
            when: d._editing
            PropertyChanges { target: input; visible: true }
        },
        State {
            name: "active"
            when: d._active
            PropertyChanges { target: current; visible: true }
        }
    ]

    // Component.onCompleted: {
    //     if (callForwarding[ruleName] || callForwarding[ruleName] === "") {
    //         d._pending = false;
    //     }
    // }

    ListItem.ThinDivider { anchors { left: parent.left; right: parent.right }}

    ListItem.Standard {
        id: control
        onClicked: check.trigger(!check.checked)
        control: CheckBox {
            id: check
            onTriggered: Utils.check(checked)
            checked: callForwarding[ruleName] !== ""
        }
    }

    ListItem.Standard {
        id: input
        visible: false
        text: i18n.tr("Forward to")
        control: TextField {
                id: field
                horizontalAlignment: TextInput.AlignRight
                inputMethodHints: Qt.ImhDialableCharactersOnly
                text: root.callForwarding[ruleName] ?
                      root.callForwarding[ruleName] : ""
                font.pixelSize: units.dp(18)
                font.weight: Font.Light
                font.family: "Ubuntu"
                color: "#AAAAAA"
                maximumLength: 20
                focus: true
                // cursorVisible: text !== serverProperty || text === ""
                cursorVisible: true
                style: TextFieldStyle {
                    overlaySpacing: units.gu(0.5)
                    frameSpacing: 0
                    background: Rectangle {
                        property bool error: (field.hasOwnProperty("errorHighlight") &&
                                             field.errorHighlight &&
                                             !field.acceptableInput)
                        onErrorChanged: error ? UbuntuColors.orange : color
                        color: Theme.palette.normal.background
                        anchors.fill: parent
                        visible: field.activeFocus
                    }
                    color: UbuntuColors.lightAubergine
                }

                cursorDelegate: Rectangle {
                    width: units.dp(1)
                    color: "#DD4814"

                }
                onVisibleChanged: if (visible === true) forceActiveFocus()
            }
    }

    /* This will show either the number of the rule, or the contact assigned
    the rule. */
    ListItem.SingleValue {
        id: current
        visible: false
        text: i18n.tr("Forward to")
        value: root.callForwarding[ruleName] ?
               root.callForwarding[ruleName] : ""
        onClicked: d._editing = true
    }

    /* Error message shown when updating fails. */
    Label {
        id: failed
        anchors {
            left: parent.left; right: parent.right; margins: units.gu(2);
        }
        visible: false
        height: contentHeight + units.gu(4)
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        color: UbuntuColors.red
        text: i18n.tr("Call forwarding can’t be changed right now.")
    }

    ActivityIndicator {
        id: activity
        running: d._pending
        visible: running
    }
}
