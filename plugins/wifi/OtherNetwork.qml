/*
 * Copyright 2013 Canonical Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 2.0
import QtQuick.Layouts 1.1
import SystemSettings 1.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import Ubuntu.Components.Popups 0.1
import Ubuntu.SystemSettings.Wifi 1.0
import QMenuModel 0.1

Component {

    Dialog {

        id: otherNetworkDialog
        objectName: "otherNetworkDialog"
        anchorToKeyboard: true

        function settingsValid() {
            if(networkname.length == 0) {
                return false;
            }
            if(securityList.selectedIndex == 0) {
                return true
            }
            if(securityList.selectedIndex == 3) {
                
            // WEP
            return password.length === 5  ||
                   password.length === 10 ||
                   password.length === 13 ||
                   password.length === 26;
        	}
		//WPA
		return password.length >= 8
        }

        title: i18n.tr("Connect to Hidden Network")
        text: feedback.enabled ? feedback.text : "";

        Common {
            id: common
        }

        states: [
        State {
            name: "CONNECTING"
            PropertyChanges {
                target: connectAction
                enabled: false
            }
            PropertyChanges {
                target: connectButtonIndicator
                running: true
            }

            PropertyChanges {
                target: p2authList
                enabled: false
            }
            PropertyChanges {
                target: p2authListLabel
                opacity: 0.5
            }
            PropertyChanges {
                target: cacert
                enabled: false
            }
            PropertyChanges {
                target: cacertLabel
                opacity: 0.5
            }
            PropertyChanges {
                target: passwordVisibleSwitch
                enabled: false
            }
            PropertyChanges {
                target: passwordVisibleLabel
                opacity: 0.5
            }
            PropertyChanges {
                target: password
                enabled: false
            }
            PropertyChanges {
                target: passwordLabel
                opacity: 0.5
            }
            PropertyChanges {
                target: username
                enabled: false
            }
            PropertyChanges {
                target: usernameLabel
                opacity: 0.5
            }
             PropertyChanges {
                target: anonymousIdentity
                enabled: false
            }
            PropertyChanges {
                target: anonymousIdentityLabel
                opacity: 0.5
            }
            PropertyChanges {
                target: authList
                enabled: false
                opacity: 0.5
            }
            PropertyChanges {
                target: authListLabel
                opacity: 0.5
            }
            PropertyChanges {
                target: securityList
                enabled: false
                opacity: 0.5
            }
            PropertyChanges {
                target: securityListLabel
                opacity: 0.5
            }
            PropertyChanges {
                target: networkname
                enabled: false
            }
            PropertyChanges {
                target: networknameLabel
                opacity: 0.5
            }
            PropertyChanges {
                target: feedback
                enabled: true
                visible: true
            }
        },
        State {
            name: "FAILED"
            PropertyChanges {
                target: feedback
                enabled: true
                visible: true
            }
        },
        State {
            name: "SUCCEEDED"
            PropertyChanges {
                target: successIndicator
                running: true
            }

            PropertyChanges {
                target: connectAction
                enabled: false
            }
        }
    ]

        Label {
            property bool enabled: false
            id: feedback
            horizontalAlignment: Text.AlignHCenter
            height: contentHeight
            wrapMode: Text.Wrap
            visible: false
        }

        Label {
                id: networknameLabel
                text : i18n.tr("Network name")
                objectName: "networknameLabel"
                fontSize: "medium"
                font.bold: false
                color: Theme.palette.selected.backgroundText
                elide: Text.ElideRight
            }

            TextField {
                id : networkname
                objectName: "networkname"
                width: parent.width
                placeholderText: i18n.tr("SSID")
                inputMethodHints: Qt.ImhNoPredictiveText
                Component.onCompleted: forceActiveFocus()
            }

            //ListItem.ThinDivider {}

            Label {
                id: securityListLabel
                text : i18n.tr("Security")
                objectName: "securityListLabel"
                fontSize: "medium"
                font.bold: false
                color: Theme.palette.selected.backgroundText
                elide: Text.ElideRight
            }

            ListItem.ItemSelector {
                id: securityList
                objectName: "securityList"
                model: [i18n.tr("None"),                 // index: 0
                        i18n.tr("WPA & WPA2 Personal"),  // index: 1
                        i18n.tr("WPA & WPA2 Enterprise"),// index: 2
                        i18n.tr("WEP"),                  // index: 3
                        i18n.tr("Dynamic WEP (802.1x)"), // index: 4
                        i18n.tr("LEAP"),                 // index: 5
                       ]
                selectedIndex: 1
            }

            Label {
                id: authListLabel
                text : i18n.tr("Authentication")
                objectName: "authListLabel"
                fontSize: "medium"
                font.bold: false
                color: Theme.palette.selected.backgroundText
                elide: Text.ElideRight
                visible: ( securityList.selectedIndex == 2 || securityList.selectedIndex == 4)
            }

            ListItem.ItemSelector {
                id: authList
                objectName: "authList"
                model: [i18n.tr("TLS"),   // index: 0
                        i18n.tr("TTLS"),  // index: 1
                        i18n.tr("LEAP"),  // index: 2
                        i18n.tr("FAST"),  // index: 3
                        i18n.tr("PEAP"),  // index: 4
                       ]
                visible: ( securityList.selectedIndex == 2 || securityList.selectedIndex == 4)
            }

            Label {
                id: p2authListLabel
                text : i18n.tr("Inner authentication")
                objectName: "p2authLabel"
                fontSize: "medium"
                font.bold: false
                color: Theme.palette.selected.backgroundText
                elide: Text.ElideRight
                visible:    ( securityList.selectedIndex == 2 || securityList.selectedIndex == 4) // WPA or D-WEP
                         && ( authList.selectedIndex == 1 ||
                              authList.selectedIndex == 3 ||
                              authList.selectedIndex == 4  )
            }

            ListItem.ItemSelector {
                id: p2authList
                objectName: "p2authList"
                width: parent.width
                model: [i18n.tr("PAP"),      // index: 0
                        i18n.tr("MSCHAPv2"), // index: 1
                        i18n.tr("MSCHAP"),   // index: 2
                        i18n.tr("CHAP"),     // index: 3
                        i18n.tr("GTC"),      // index: 4
                        i18n.tr("MD5")       // index: 5
                       ]
                visible:    ( securityList.selectedIndex == 2 || securityList.selectedIndex == 4) // WPA or D-WEP
                         && ( authList.selectedIndex == 1 ||
                              authList.selectedIndex == 3 ||
                              authList.selectedIndex == 4  )
            }

            Column{     // ca-cert
                id: cacertColumn
                anchors {
                    left: parent.left
                    right: parent.right
                }
                spacing: parent.spacing

                visible:    ( securityList.selectedIndex == 2 || securityList.selectedIndex == 4)
                         && ( authList.selectedIndex == 0 ||
                              authList.selectedIndex == 1 ||
                              authList.selectedIndex == 4  )

                RowLayout{
                    spacing: units.gu(4)
                    anchors {
                        left: parent.left
                        right: parent.right
                    }

                    Label {
                        id: cacertLabel
                        text : i18n.tr("CA Certificate")
                        objectName: "cacertListLabel"
                        fontSize: "medium"
                        font.bold: false
                        color: Theme.palette.selected.backgroundText
                        //anchors.bottom: addcacertButton.bottom // while button is disabled
                    }

                    Button {
                        id: addcacertButton
                        //action: selectPeer
                        visible: false //showAllUI // Button action not implemented yet.
                        objectName: "addcacertButton"
                        anchors.right: parent.right
                        text: i18n.tr("Choose file…")
                    }
                }


                TextArea  {
                    id : cacert
                    objectName: "cacert"
                    width: parent.width
                    autoSize: true
                    maximumLineCount: 4
                    placeholderText: i18n.tr("Absolute path to cert file or clipboard content")

                }

                RowLayout{
                    spacing: units.gu(4)
                    anchors {
                        left: parent.left
                        right: parent.right
                    }
                    visible:    ( securityList.selectedIndex == 2 || securityList.selectedIndex == 4)
                             && ( authList.selectedIndex == 0 ) // only for TLS

                    Label {
                        id: usercertLabel
                        text : i18n.tr("User Certificate")
                        objectName: "usercertListLabel"
                        fontSize: "medium"
                        font.bold: false
                        color: Theme.palette.selected.backgroundText
                        //anchors.bottom: addusercertButton.bottom
                    }

                    Button {
                        id: addusercertButton
                        //action: selectPeer
                        visible: false // Button action not implemented yet.
                        objectName: "addusercertButton"
                        anchors.right: parent.right
                        text: i18n.tr("Choose file…")
                    }
                }

                TextArea{
                    id : usercert
                    objectName: "usercert"
                    visible:    ( securityList.selectedIndex == 2 || securityList.selectedIndex == 4)
                             && ( authList.selectedIndex == 0 )
                    width: parent.width
                    autoSize: true
                    maximumLineCount: 4
                    placeholderText: i18n.tr("Absolute path to cert file or clipboard content")
                }


                RowLayout{
                    spacing: units.gu(4)
                    anchors {
                        left: parent.left
                        right: parent.right
                    }
                    visible:    ( securityList.selectedIndex == 2 || securityList.selectedIndex == 4)
                             && ( authList.selectedIndex == 0 ) // only for TLS

                    Label {
                        id: userprivatekeyLabel
                        text : i18n.tr("User Private Key")
                        objectName: "userprivatekeyLabel"
                        fontSize: "medium"
                        font.bold: false
                        color: Theme.palette.selected.backgroundText
                        //anchors.bottom: adduserprivatekeyButton.bottom
                    }

                    Button {
                        id: adduserprivatekeyButton
                        //action: selectPeer
                        visible: false // Button action not implemented yet
                        objectName: "adduserprivatekeyButton"
                        anchors.right: parent.right
                        text: i18n.tr("Choose file…")
                    }
                }

                TextArea {
                    id : userprivatekey
                    objectName: "userprivatekey"
                    visible:    ( securityList.selectedIndex == 2 || securityList.selectedIndex == 4)
                             && ( authList.selectedIndex == 0 )
                    width: parent.width
                    autoSize: true
                    maximumLineCount: 4
                    placeholderText: i18n.tr("Absolute path to key or clipboard content")
                }

                //ListItem.ThinDivider {}

            }

            Column{     // pacFile
                id: pacFileColumn
                anchors {
                    left: parent.left
                    right: parent.right
                }
                spacing: parent.spacing

                visible:    ( securityList.selectedIndex == 2 || securityList.selectedIndex == 4)
                         && ( authList.selectedIndex == 3 )

                RowLayout{
                    spacing: units.gu(4)
                    anchors {
                        left: parent.left
                        right: parent.right
                    }

                    Label {
                        id: pacFileLabel
                        text : i18n.tr("Pac File")
                        objectName: "pacFileLabel"
                        fontSize: "medium"
                        font.bold: false
                        color: Theme.palette.selected.backgroundText
                        //anchors.bottom: adduserprivatekeyButton.bottom
                    }

                    Button {
                        id: addpacFileButton
                        //action: selectPeer
                        visible: false // Button action not implemented yet
                        objectName: "addpacFileButton"
                        anchors.right: parent.right
                        text: i18n.tr("Add file…")
                    }
                }

                TextArea {
                    id : pacFile
                    objectName: "pacFile"
                    width: parent.width
                    autoSize: true
                    maximumLineCount: 4
                    placeholderText: i18n.tr("Absolute path to Pac File or clipboard content")
                }

                //ListItem.ThinDivider {}

            }

            Label {
                id: anonymousIdentityLabel
                text : i18n.tr("Anonymous identity")
                objectName: "anonymousIdentityLabel"
                fontSize: "medium"
                font.bold: false
                color: Theme.palette.selected.backgroundText
                elide: Text.ElideRight
                visible: ( securityList.selectedIndex == 2  && authList.selectedIndex !== 2 )
            }

            TextField {
                id : anonymousIdentity
                objectName: "anonymousIdentity"
                width: parent.width
                visible: ( securityList.selectedIndex == 2  && authList.selectedIndex !== 2 )
                inputMethodHints: Qt.ImhNoPredictiveText
                Component.onCompleted: forceActiveFocus()
                onAccepted: {
                    connectAction.trigger()
                }
            }

            Label {
                id: usernameLabel
                text : {
                         if (     ( securityList.selectedIndex == 2 || securityList.selectedIndex == 4)
                               && ( authList.selectedIndex == 0 )) {
                             i18n.tr("Identity")
                         }
                         else {
                             i18n.tr("Username")
                         }
                }
                objectName: "usernameLabel"
                fontSize: "medium"
                font.bold: false
                color: Theme.palette.selected.backgroundText
                elide: Text.ElideRight
                visible: ( securityList.selectedIndex == 2 || securityList.selectedIndex == 4 || securityList.selectedIndex == 5)
            }

            TextField {
                id : username
                objectName: "username"
                width: parent.width
                visible: ( securityList.selectedIndex == 2 || securityList.selectedIndex == 4 || securityList.selectedIndex == 5)
                inputMethodHints: Qt.ImhNoPredictiveText
                Component.onCompleted: forceActiveFocus()
                onAccepted: {
                    connectAction.trigger()
                }
            }

            Label {
                id: passwordLabel
                text: {
                         if (    ( securityList.selectedIndex == 2 || securityList.selectedIndex == 4)
                             && ( authList.selectedIndex == 0 )) {
                            i18n.tr("Private Key Password")
                         } else {
                            i18n.tr("Password")
                         }
                }

                objectName: "passwordListLabel"
                fontSize: "medium"
                font.bold: false
                color: Theme.palette.selected.backgroundText
                elide: Text.ElideRight
                visible: securityList.selectedIndex !== 0
            }

            TextField {
                id : password
                objectName: "password"
                width: parent.width
                visible: securityList.selectedIndex !== 0
                echoMode: passwordVisibleSwitch.checked ?
                              TextInput.Normal : TextInput.Password
                inputMethodHints: Qt.ImhNoPredictiveText
                onAccepted: {
                    connectAction.trigger();
                }
            }

            Row {
                id: passwordVisiblityRow
                layoutDirection: Qt.LeftToRight
                spacing: units.gu(2)
                visible: securityList.selectedIndex !== 0

                CheckBox {
                    id: passwordVisibleSwitch
                    //activeFocusOnPress: false
                }

                Label {
                    id: passwordVisibleLabel
                    text : i18n.tr("Show password")
                    objectName: "passwordVisibleLabel"
                    fontSize: "medium"
                    color: Theme.palette.selected.backgroundText
                    elide: Text.ElideRight
                    height: passwordVisibleSwitch.height
                    verticalAlignment: Text.AlignVCenter
                    MouseArea {
                        anchors {
                            fill: parent
                        }
                        onClicked: {
                            passwordVisibleSwitch.checked =
                                    !passwordVisibleSwitch.checked
                        }
                    }
                }
           }

           Row {
                    id: passwordRememberRow
                    layoutDirection: Qt.LeftToRight
                    spacing: units.gu(2)
                    visible: false
                             //( authList.selectedIndex == 1 || authList.selectedIndex == 3 || authList.selectedIndex == 4)
                             //not implemented yet.
                    CheckBox {
                        id: passwordRememberSwitch
                        //activeFocusOnPress: false
                    }

                    Label {
                        id: passwordRememberLabel
                        text : i18n.tr("Remember password")
                        objectName: "passwordRememberLabel"
                        fontSize: "medium"
                        color: Theme.palette.selected.backgroundText
                        elide: Text.ElideRight
                        height: passwordRememberSwitch.height
                        verticalAlignment: Text.AlignVCenter
                        MouseArea {
                            anchors {
                                fill: parent
                            }
                            onClicked: {
                                passwordRememberSwitch.checked =
                                        !passwordRememberSwitch.checked
                            }
                        }
                    }

        }
        //ListItem.ThinDivider {visible: securityList.selectedIndex != 0}
        RowLayout {
            id: buttonRow
            anchors {
                left: parent.left
                right: parent.right
            }
            spacing: units.gu(2)
            height: cancelButton.height

            Button {
                id: cancelButton
                objectName: "cancel"
                Layout.fillWidth: true
                text: i18n.tr("Cancel")
                onClicked: {
                    PopupUtils.close(otherNetworkDialog);

                    // If this dialog created the connection,
                    // disconnect the device
                    if (otherNetworkDialog.state === "CONNECTING") {
                        DbusHelper.forgetActiveDevice();
                    }
                }
            }

            Button {
                id: connectButton
                objectName: "connect"
                Layout.fillWidth: true
                text: i18n.tr("Connect")
                enabled: connectAction.enabled
                action: connectAction
                Icon {
                    height: parent.height - units.gu(1.5)
                    width: parent.height - units.gu(1.5)
                    anchors {
                        centerIn: parent
                    }
                    name: "tick"
                    color: "green"
                    visible: successIndicator.running
                }
                ActivityIndicator {
                    id: connectButtonIndicator
                    running: false
                    visible: running
                    height: parent.height - units.gu(1.5)
                    anchors {
                        centerIn: parent
                    }
                }
            }
        }

        Action {
            id: connectAction
            enabled: settingsValid()
            onTriggered: {
                DbusHelper.connect(
                    networkname.text,
                    securityList.selectedIndex,
                    authList.selectedIndex,
                                [username.text, anonymousIdentity.text],
                                password.text,
                                [cacert.text, usercert.text, userprivatekey.text, pacFile.text] ,
                                p2authList.selectedIndex);
                otherNetworkDialog.state = "CONNECTING";
            }
        }

        /* Timer that shows a tick in the connect button once we have
        successfully connected. */
        Timer {
            id: successIndicator
            interval: 2000
            running: false
            repeat: false
            onTriggered: PopupUtils.close(otherNetworkDialog)
        }

        Connections {
            target: DbusHelper
            onDeviceStateChanged: {
                if (otherNetworkDialog.state === "FAILED") {
                    /* Disconnect the device if it tries to reconnect after a
                    connection failure */
                    if (newState === 40) { // 40 = NM_DEVICE_STATE_PREPARE
                        DbusHelper.forgetActiveDevice();
                    }
                }

                /* We will only consider these cases if we are in
                the CONNECTING state. This means that this Dialog will not
                react to what other NetworkManager consumers do.
                */
                if (otherNetworkDialog.state === "CONNECTING") {
                    switch (newState) {
                        case 120:
                            feedback.text = common.reasonToString(reason);
                            otherNetworkDialog.state = "FAILED";
                            break;
                        case 100:
                            /* connection succeeded only if it was us that
                            created it */
                            otherNetworkDialog.state = "SUCCEEDED";
                            break;
                    }
                }
            }
        }
    }
}

