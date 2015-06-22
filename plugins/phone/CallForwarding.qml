/*
 * This file is part of system-settings
 *
 * Copyright (C) 2013 Canonical Ltd.
 *
 * Contact: Sebastien Bacher <sebastien.bacher@canonical.com>
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
import QtContacts 5.0
import SystemSettings 1.0
import Ubuntu.Components 1.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import Ubuntu.Components.Popups 0.1
import Ubuntu.Components.Themes.Ambiance 0.1
import Ubuntu.Content 0.1

ItemPage {
    id: page
    objectName: "callForwardingPage"
    title: headerTitle
    property var sim
    property string headerTitle: i18n.tr("Call forwarding")
    // Holds the forwarding item we're currently editing.
    property var editing: null
    property list<ContentItem> importItems
    property var activeTransfer

    states: [
        // State {
        //     name: "forwardCheck"
        //     PropertyChanges { target: fwdAll; control: allAct }
        //     PropertyChanges { target: fwdSomeTitle; }

        //     PropertyChanges { target: fwdBusy; control: busyAct }

        //     PropertyChanges { target: fwdLost; control: lostAct }

        //     PropertyChanges { target: fwdUnreachable; control: unreachableAct }
        // },
        State {
            name: "forwardFailed"
            PropertyChanges { target: fwdSomeTitle; }
            PropertyChanges { target: fwdFailedLabel; visible: true }
            PropertyChanges { target: fwdAllCaption; }

            PropertyChanges { target: fwdAll; enabled: false; }
            PropertyChanges { target: fwdBusy; enabled: false; }
            PropertyChanges { target: fwdLost; enabled: false; }
            PropertyChanges { target: fwdUnreachable; enabled: false; }
        },
        State {
            name: "editing"
            PropertyChanges { target: fwdAll; enabled: false; explicit: true }
            PropertyChanges { target: fwdBusy; enabled: false; explicit: true }
            PropertyChanges { target: fwdLost; enabled: false; explicit: true }
            PropertyChanges { target: fwdUnreachable; enabled: false; explicit: true }
            PropertyChanges { target: fwdSomeTitle; }
            StateChangeScript {
                name: "editingEnabled"
                script: {
                    editing.opacity = 1;
                    editing.enabled = true;
                }
            }
            when: editing !== null
        },
        State {
            name: "forwardAll"
            PropertyChanges { target: fwdSomeTitle; }
            PropertyChanges { target: fwdBusy; enabled: false; value: ""; checked: false }
            PropertyChanges { target: fwdLost; enabled: false; value: ""; checked: false }
            PropertyChanges { target: fwdUnreachable; enabled: false; value: ""; checked: false }
            when: fwdAll.value !== ""
        }
        // State {
        //     name: "forwardSome"
        //     // PropertyChanges { target: fwdAll; }
        //     // PropertyChanges { target: fwdAllCheck; enabled: false }
        // }
    ]

    // We need to disable keyboard anchoring because we implement the
    // KeyboardRectangle pattern
    Binding {
        target: main
        property: "anchorToKeyboard"
        value: false
    }

    // onEditingChanged: {
    //     console.warn('onEditingChanged', editing);
    //     [fwdAll, fwdBusy, fwdLost, fwdUnreachable].forEach(function (n) {
    //         if (editing) {
    //             if (n !== editing) {
    //                 n.enabled = false;
    //             }
    //         } else {
    //             n.enabled = true;
    //         }
    //     });
    // }

    flickable: null
    Flickable {
        id: flick

        // this is necessary to avoid the page to appear below the header
        clip: true
        flickableDirection: Flickable.VerticalFlick
        anchors {
            fill: parent
            bottomMargin: keyboardButtons.height + keyboard.height
        }
        contentHeight: contents.height + units.gu(2)
        contentWidth: parent.width

        Column {
            id: contents
            anchors { left: parent.left; right: parent.right }
            spacing: units.gu(1)

            CallForwardItem {
                id: fwdAll
                anchors { left: parent.left; right: parent.right }
                rule: "voiceUnconditional"
                callForwarding: sim.callForwarding
                text: i18n.tr("Forward every incoming call")
                onEnteredEditMode: page.editing = fwdAll
                onLeftEditMode: page.editing = null
            }

            Label {
                id: fwdAllCaption
                anchors {
                    left: parent.left; right: parent.right; margins: units.gu(1)
                }
                width: parent.width
                wrapMode: Text.WordWrap
                fontSize: "small"
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                text: i18n.tr("Redirects all phone calls to another number.")
                opacity: 0.8
            }

            Label {
                id: fwdFailedLabel
                anchors {
                    left: parent.left; right: parent.right; margins: units.gu(2)
                }
                width: parent.width
                wrapMode: Text.WordWrap
                visible: false
                text: i18n.tr("Call forwarding status can’t be checked " +
                              "now. Try again later.")
                color: UbuntuColors.red
                horizontalAlignment: Text.AlignHCenter
            }

            SettingsItemTitle {
                id: fwdSomeTitle
                text: i18n.tr("Forward incoming calls when:")
                showDivider: false
            }

            CallForwardItem {
                id: fwdBusy
                anchors { left: parent.left; right: parent.right }
                callForwarding: sim.callForwarding
                rule: "voiceBusy"
                text: i18n.tr("I’m on another call")
                onEnteredEditMode: page.editing = fwdBusy
                onLeftEditMode: page.editing = null
                // onEditing: page.editing = fwdBusy
                // onStoppedEditing: page.editing = null
                // onRequestRuleChange: sim.callForwarding.voiceBusy = value
                // Component.onCompleted: value = sim.callForwarding.voiceBusy
            }

            CallForwardItem {
                id: fwdLost
                anchors { left: parent.left; right: parent.right }
                callForwarding: sim.callForwarding
                rule: "voiceNoReply"
                text: i18n.tr("I don’t answer")
                onEnteredEditMode: page.editing = fwdLost
                onLeftEditMode: page.editing = null
                // onEditing: page.editing = fwdLost
                // onStoppedEditing: page.editing = null
                // onRequestRuleChange: sim.callForwarding.voiceNoReply = value
                // Component.onCompleted: value = sim.callForwarding.voiceNoReply
            }

            CallForwardItem {
                id: fwdUnreachable
                anchors { left: parent.left; right: parent.right }
                callForwarding: sim.callForwarding
                rule: "voiceNotReachable"
                text: i18n.tr("My phone is unreachable")
                onEnteredEditMode: page.editing = fwdUnreachable
                onLeftEditMode: page.editing = null
                // onEditing: page.editing = fwdUnreachable
                // onStoppedEditing: page.editing = null
                // onRequestRuleChange: sim.callForwarding.voiceNotReachable = value
                // Component.onCompleted: value = sim.callForwarding.voiceNotReachable
            }

            Label {
                property string forwardFlag: sim.callForwarding.forwardingFlagOnSim ? 'ON' : 'OFF'
                property string voiceUnconditional: sim.callForwarding.voiceUnconditional ?
                                            sim.callForwarding.voiceUnconditional : '(empty)'
                property string voiceBusy: sim.callForwarding.voiceBusy ?
                                            sim.callForwarding.voiceBusy : '(empty)'
                property string voiceNoReply: sim.callForwarding.voiceNoReply ?
                                            sim.callForwarding.voiceNoReply : '(empty)'
                property string voiceNotReachable: sim.callForwarding.voiceNotReachable ?
                                            sim.callForwarding.voiceNotReachable : '(empty)'
                anchors { left: parent.left; right: parent.right; }
                wrapMode: Text.WrapAnywhere
                text: "forwardingFlagOnSim: " + forwardFlag +
                      "\nvoiceUnconditional: " + voiceUnconditional +
                      "\nvoiceBusy: " + voiceBusy +
                      "\nvoiceNoReply: " + voiceNoReply +
                      "\nvoiceNotReachable: " + voiceNotReachable +
                      "\n\nfwdAll = " + fwdAll.enabled.toString() +
                      "\nfwdBusy.enabled = " + fwdBusy.enabled.toString() +
                      "\nfwdLost.enabled = " + fwdLost.enabled.toString() +
                      "\nfwdUnreachable.enabled = " + fwdUnreachable.enabled.toString() +
                      "\nfwdAll.opacity = " + fwdAll.opacity.toString() +
                      "\nfwdBusy.opacity = " + fwdBusy.opacity.toString() +
                      "\nfwdLost.opacity = " + fwdLost.opacity.toString() +
                      "\nfwdUnreachable.opacity = " + fwdUnreachable.opacity.toString() +
                      "\n\nstate: " + page.state +
                      "\nediting: " + page.editing +
                      "\ncontent hub items: " + page.importItems +
                      "\ncontent hub items len: " + page.importItems.length
            }
        }
    } // Flickable

    Rectangle {
        id: keyboardButtons
        anchors {
            left: parent.left
            right: parent.right
            bottom: keyboard.top
        }
        color: Theme.palette.selected.background
        visible: editing !== null
        height: units.gu(6)
        Button {
            id: kbdContacts
            anchors {
                left: parent.left
                leftMargin: units.gu(1)
                verticalCenter: parent.verticalCenter
            }
            activeFocusOnPress: false
            enabled: editing && !editing.busy
            text: i18n.tr("Contacts…")
            onClicked: page.activeTransfer = contactPicker.request()
        }

        Button {
            id: kbdCancel
            anchors {
                right: kbdSet.left
                rightMargin: units.gu(1)
                verticalCenter: parent.verticalCenter
            }
            enabled: editing && !editing.busy
            text: i18n.tr("Cancel")
            onClicked: editing.cancel()
        }

        Button {
            id: kbdSet
            anchors {
                right: parent.right
                rightMargin: units.gu(1)
                verticalCenter: parent.verticalCenter
            }
            enabled: editing && !editing.busy
            text: i18n.tr("Set")
            onClicked: editing.save()
        }
    }

    KeyboardRectangle {
        id: keyboard
        anchors.bottom: parent.bottom
        onHeightChanged: {
            console.warn('onHeightChanged...');
        }
    }


    Component {
        id: forwardContact
        Contact {
            id: contct
            property PhoneNumber forwardToNumber
        }
    }


    Component {
        id: chooseNumberDialog
        Dialog {
            id: dialog
            property var contact
            title: i18n.tr('Please select a phone number')

            ListItem.ItemSelector {
                anchors {
                    left: parent.left
                    right: parent.right
                }
                expanded: true
                text: i18n.tr("Numbers")
                model: contact.phoneNumbers
                delegate: OptionSelectorDelegate {
                    text: modelData.number
                    activeFocusOnPress: false
                }
                onDelegateClicked: {
                    console.warn('selected number', index, contact.phoneNumbers[index]);
                    contact.forwardToNumber = contact.phoneNumbers[index];
                    onClicked: {
                        editing.setContact(contact);
                        PopupUtils.close(dialog)
                    }
                }
            }
        }
    }

    Component {
        id: hadNoNumberDialog
        Dialog {
            id: dialog
            title: i18n.tr("Could not forward to this contact")
            text: i18n.tr("Contact not associated with any phone number.")
            Button {
                text: i18n.tr("OK")
                activeFocusOnPress: false
                onClicked: PopupUtils.close(dialog)
            }
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
            return vcardContact;
        }

        onVcardParsed: {
            console.warn('onVcardParsed', contacts);
            var contact;
            if (contacts.length === 0) {
                console.warn('no contacts parsed');
                return;
            } else {
                console.warn('onVcardParsed parsing contact...');
                contact = parseContact(contacts[0]);
                console.warn('onVcardParsed parsed contact', contact);
                if (contact.phoneNumbers.length === 0) {
                    console.log("telling user there was no number", contact);
                    PopupUtils.open(hadNoNumberDialog);
                } else if (contact.phoneNumbers.length > 1) {
                    console.log("prompting for selection of number", contact);
                    PopupUtils.open(chooseNumberDialog, page, {
                        'contact': contact
                    });
                } else {
                    console.log("using contact", contact);
                    contact.forwardToNumber = contact.PhoneNumber;
                    editing.setContact(contact);
                }
                console.log("Parsed contact", contact);
            }
        }
    }

    ContentTransferHint {
        id: importHint
        anchors.fill: parent
        activeTransfer: page.activeTransfer
    }

    ContentPeer {
        id: contactPicker
        contentType: ContentType.Contacts
        handler: ContentHandler.Source
        selectionType: ContentTransfer.Single
    }

    Connections {
        target: page.activeTransfer ? page.activeTransfer : null
        onStateChanged: {
            if (page.activeTransfer.state === ContentTransfer.Charged) {
                console.warn('ContentTransfer.Charged');
                contactParser.vCardUrl = page.activeTransfer.items[0].url;
            }
        }
    }

    Connections {
        target: sim.callForwarding
        onGetPropertiesFailed: {
            console.warn('failed');
            root.state = "forwardFailed";
        }
    }
}
