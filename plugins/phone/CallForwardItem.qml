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
import QtContacts 5.0
import "callForwardingUtils.js" as Utils

Column {
    id: item

    /**
     * Rule.
     */
    property OfonoCallForwarding callForwarding
    property bool enabled: true
    property string rule

    property alias checked: check.checked
    property alias busy: d._pending
    property alias text: control.text
    property alias value: current.value

    signal checked ()
    signal failed ()
    signal enteredEditMode ()
    signal leftEditMode ()
    signal serverResponse (bool success)

    function save () {
        console.warn('save');
        d._pending = true;
        if (!Utils.requestRule(field.text)) {
            d._pending = false;
            d._editing = false;
            checked: callForwarding[rule] !== "";
        }
    }

    function cancel () {
        console.warn('cancel')
        d._editing = false;
    }

    function importContact (contentItem) {
        console.warn('importing contact', contentItem.url);
        contactParser.vCardUrl = contentItem.url;
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
         * We're editing.
         */
        property bool _editing: false
        on_EditingChanged: Utils.editingChanged()

        /**
         * Whether or not the forwarding rule is active.
         */
        property bool _active: callForwarding[rule] !== ""
    }

    states: [
        State {
            name: "failed"
            when: d._failed
            PropertyChanges { target: control; enabled: false; control: check }
            PropertyChanges { target: check; checked: false }
            PropertyChanges { target: failed; visible: true }
            PropertyChanges { target: activity; visible: false }
        },
        State {
            name: "disabled"
            when: !enabled
            PropertyChanges { target: control; enabled: false }
            PropertyChanges { target: check; enabled: false }
            PropertyChanges { target: current; enabled: false }
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
            PropertyChanges { target: check; enabled: false }
            PropertyChanges { target: current; visible: false }
            PropertyChanges { target: input; visible: true }
        },
        State {
            name: "active"
            when: d._active
            PropertyChanges { target: current; visible: true }
        }
    ]

    // Component.onCompleted: {
    //     if (rule || rule === "") {
    //         d._pending = false;
    //     }
    // }

    ListItem.ThinDivider { anchors { left: parent.left; right: parent.right }}

    ListItem.Standard {
        id: control
        onClicked: check.trigger(!check.checked)
        control: CheckBox {
            id: check
            checked: callForwarding[rule] !== ""
            onTriggered: Utils.checked(checked)
        }
    }

    ListItem.Standard {
        id: input
        visible: false
        height: visible ? units.gu(6) : 0
        text: i18n.tr("Forward to")
        control: TextField {
            id: field
            horizontalAlignment: TextInput.AlignRight
            inputMethodHints: Qt.ImhDialableCharactersOnly
            text: callForwarding[rule]
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

        Behavior on height {
            NumberAnimation {
                duration: UbuntuAnimation.SnapDuration
            }
        }
    }

    /* This will show either the number of the rule, or the contact assigned
    the rule. */
    ListItem.SingleValue {
        id: current
        visible: value
        text: i18n.tr("Forward to")
        value: callForwarding[rule]
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

    Connections {
        target: item
        Component.onCompleted: {
            item.callForwarding[item.rule + 'Changed'].connect(Utils.ruleChanged);
            item.callForwarding[item.rule + 'Complete'].connect(Utils.ruleComplete);
                    // onVoiceUnconditionalChanged: {
                    //     parent.value = property;
                    //     console.warn('voiceUnconditional changed', property);
                    // }
                    // onVoiceUnconditionalComplete: {
                    //     console.warn('voiceUnconditional complete', success);
                    //     if (success) {
                    //         // worked
                    //     } else {
                    //         // failed
                    //     }
                    // }
        }
        Component.onDestruction: {
            item.callForwarding[item.rule + 'Changed'].disconnect(Utils.ruleChanged);
            item.callForwarding[item.rule + 'Complete'].disconnect(Utils.ruleComplete);
        }
    }

    Component {
        id: forwardContact
        Contact {
            id: contct
        }
    }


    VCardParser {
        id: contactParser

        signal addContactsAnswer(var id, var contactIds, var retryList, var urserList);

        property int importedContactCount: 0
        property string dialogTitle: ""
        property string dialogText: ""

        function parseContact(vcardContact) {
            console.warn('parseContact', vcardContact);
            var contact = forwardContact.createObject(contactParser);

            contact.name.firstName = vcardContact.name.firstName;
            contact.name.lastName = vcardContact.name.lastName;
            if (contact.name.firstName === "") {
                var labelName = vcardContact.displayLabel.label.split(" ");
                contact.firstName = labelName[0];
                if (labelName.length >1) {
                    //removes the first name
                    labelName.shift();
                    contact.lastName = labelName.toString().replace(",","");
                } else {
                    contact.lastName = "";
                }
            }
            contact.phone = vcardContact.phoneNumber.number;
            return contact;
        }

        // onAddContactsAnswer: {
        //     telegramClient.contactsImportContactsAnswer.disconnect(contactParser.addContactsAnswer);
        //     busy = false;

        //     console.log("Imported " + contactIds.length + " contacts out of " + importedContactCount);
        //     dialogTitle = contactIds.length > 0 ? i18n.tr("Contacts imported") : i18n.tr("No contacts imported");
        //     dialogText = i18n.tr("This contact is on Telegram.",
        //                          "%1 out of %2 contacts are on Telegram.",
        //                          importedContactCount).arg(contactIds.length).arg(importedContactCount);
        //     PopupUtils.open(contactImportDialogComponent);
        // }

        onVcardParsed: {
            if (contacts.length === 0) {
                console.warn('no contacts parsed');
                return;
            }
            console.log("Parsed " + contacts.length + " contacts.");
            if (contacts.length === 1) {
                var singleContact = parseContact(contacts[0]);
                // showAddContactPage(true, -1, singleContact);
                console.warn('parsed single contact', singleContact)
            } else {
                var contactList = [];
                for (var i = 0; i < contacts.length; i++) {
                    var contact = parseContact(contacts[i]);
                    if (contact.phone !== "" && contact.firstName !== "") {
                        contactList.push(contact);
                    }
                }
                console.warn('parsed multiple contacts', contacts.length, contactList)
            }
        }
    }
}
