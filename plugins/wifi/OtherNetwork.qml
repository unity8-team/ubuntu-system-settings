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

        function settingsValid () {
            if (networkname.length === 0) {
                return false;
            }
            if (securityList.selectedIndex === 0) {
                return true
            }
            if (securityList.selectedIndex === 3) {
                // WEP
                return password.length === 5  ||
                       password.length === 10 ||
                       password.length === 13 ||
                       password.length === 26;
            }
            //WPA
            return password.length >= 8
        }

        function filePicker (type) {
            var pickerDialog;
            var certDialog;

            pickerDialog = PopupUtils.open(
                Qt.resolvedUrl("./CertPicker.qml")
            );
            pickerDialog.fileImportSignal.connect(function (file) {
                if (!file === false) {
                    certDialogLoader.source = Qt.resolvedUrl(
                        "./CertDialog.qml"
                    );
                    certDialog = PopupUtils.open(
                        certDialogLoader.item, authListLabel, {
                            fileName: file,
                            certType: type
                        }
                    );
                    certDialog.updateSignal.connect(function (update) {
                        if (update && type === 0) {
                            cacertListModel.dataupdate();
                        } else if (update && type === 1) {
                            privatekeyListModel.dataupdate();
                        } else if (update && type === 2) {
                            pacFileListModeL.dataupdate();
                        }
                    });
                }
            });
        }

        title: i18n.tr("Connect to Hidden Network")

        Label {
            property bool enabled: false
            id: feedback
            visible: false
        }


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
                    target: passwordRememberSwitch
                    enabled: false
                }
                PropertyChanges {
                    target: passwordRememberLabel
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
                    target: pacProvisioningList
                    enabled: false
                    opacity: 0.5
                }
                PropertyChanges {
                    target: pacProvisioningListLabel
                    opacity: 0.5
                }
                PropertyChanges {
                    target: pacFileSelector
                    enabled: false
                    opacity: 0.5
                }
                PropertyChanges {
                    target: pacFileLabel
                    opacity: 0.5
                }
                PropertyChanges {
                    target: privateKeySelector
                    enabled: false
                    opacity: 0.5
                }
                PropertyChanges {
                    target: privatekeyLabel
                    opacity: 0.5
                }
                PropertyChanges {
                    target: usercertSelector
                    enabled: false
                    opacity: 0.5
                }
                PropertyChanges {
                    target: usercertLabel
                    opacity: 0.5
                }
                PropertyChanges {
                    target: cacertSelector
                    enabled: false
                    opacity: 0.5
                }
                PropertyChanges {
                    target: cacertLabel
                    opacity: 0.5
                }
                PropertyChanges {
                    target: p2authList
                    enabled: false
                    opacity: 0.5
                }
                PropertyChanges {
                    target: p2authListLabel
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
                    target: wepInsecureLabel
                    opacity: 0.5
                }
                PropertyChanges {
                    target: wepInsecureLabel
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
                    enabled: false
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
            model: [i18n.tr("None"),             // index: 0
                i18n.tr("WPA & WPA2 Personal"),  // index: 1
                i18n.tr("WPA & WPA2 Enterprise"),// index: 2
                i18n.tr("WEP"),                  // index: 3
                i18n.tr("Dynamic WEP (802.1x)"), // index: 4
                i18n.tr("LEAP"),                 // index: 5
            ]
            selectedIndex: 1
        }

        Label {
            id: wepInsecureLabel
            objectName: "wepInsecureLabel"
            color: "red"
            text: i18n.tr("This network is insecure.")
            visible: securityList.selectedIndex === 3
        }

        Label {
            id: authListLabel
            text : i18n.tr("Authentication")
            objectName: "authListLabel"
            fontSize: "medium"
            font.bold: false
            color: Theme.palette.selected.backgroundText
            elide: Text.ElideRight
            visible: securityList.selectedIndex === 2 ||
                     securityList.selectedIndex === 4
        }

        ListItem.ItemSelector {
            id: authList
            objectName: "authList"
            model: ["TLS",  // index: 0
                    "TTLS", // index: 1
                    "LEAP", // index: 2
                    "FAST", // index: 3
                    "PEAP", // index: 4
            ]
            visible: securityList.selectedIndex === 2 ||
                     securityList.selectedIndex === 4
        }

        Label {
            id: p2authListLabel
            text : i18n.tr("Inner authentication")
            objectName: "p2authLabel"
            fontSize: "medium"
            font.bold: false
            color: Theme.palette.selected.backgroundText
            elide: Text.ElideRight
            visible: (securityList.selectedIndex === 2 ||
                      securityList.selectedIndex === 4 /* WPA or D-WEP */) &&
                     (authList.selectedIndex === 1 ||
                      authList.selectedIndex === 3 ||
                      authList.selectedIndex === 4)
        }

        ListItem.ItemSelector {
            id: p2authList
            objectName: "p2authList"
            width: parent.width
            model: ["PAP",      // index: 0
                    "MSCHAPv2", // index: 1
                    "MSCHAP",   // index: 2
                    "CHAP",     // index: 3
                    "GTC",      // index: 4
                    "MD5"       // index: 5
            ]
            visible: (securityList.selectedIndex === 2 ||
                      securityList.selectedIndex === 4 /* WPA or D-WEP */) &&
                     (authList.selectedIndex === 1 ||
                      authList.selectedIndex === 3 ||
                      authList.selectedIndex === 4)
        }

        Label {
            id: cacertLabel
            text : i18n.tr("CA certificate")
            objectName: "cacertListLabel"
            fontSize: "medium"
            font.bold: false
            color: Theme.palette.selected.backgroundText
            visible: (securityList.selectedIndex === 2 ||
                      securityList.selectedIndex === 4 /* WPA or D-WEP */) &&
                     (authList.selectedIndex === 0 ||
                      authList.selectedIndex === 1 ||
                      authList.selectedIndex === 3)
        }

        ListItem.ItemSelector {
            id: cacertSelector
            anchors {
                left: parent.left
                right: parent.right
            }
            visible: (securityList.selectedIndex === 2 ||
                      securityList.selectedIndex === 4 /* WPA or D-WEP */) &&
                     (authList.selectedIndex === 0 ||
                      authList.selectedIndex === 1 ||
                      authList.selectedIndex === 3)
            model: cacertListModel
            expanded: false
            delegate: certSelectorDelegate
            selectedIndex: 0
            property string cacertFileName: {
                if (selectedIndex !== 0 &&
                    selectedIndex !== (cacertListModel.rowCount()-1)) {
                    return cacertListModel.getfileName(selectedIndex);
                } else {
                    return "";
                }
            }
            onSelectedIndexChanged: {
                if (selectedIndex === cacertListModel.rowCount()-1) {
                    selectedIndex = 0;
                    otherNetworkDialog.filePicker(0); //Certificate
                }
            }
        }

        Component{
            id: certSelectorDelegate
            OptionSelectorDelegate {

                /* FIXME: This should be dealt with by the ui toolkit. We can
                not be sure that ellipsis translates well into other languages.
                */
                text: CommonName.length > 32 ?
                      CommonName.substr(0,30).concat("…") :
                      CommonName

                // FIXME: See above comment.
                // FIXME: Increase readability by using conditionals in a block.
                subText: (CommonName !== i18n.tr("None") &&
                          CommonName !== i18n.tr("Choose…")) ?
                          (((Organization.length > 15) ?
                             Organization.substr(0,13).concat("…") :
                             Organization)
                          + ", " + i18n.tr("Exp.date: ") + expiryDate) : ""
            }
        }

        CertificateListModel {
            id: cacertListModel
        }

        Loader {
            id: certDialogLoader
            asynchronous: false
        }

        Label {
            id: usercertLabel
            text : i18n.tr("Client certificate")
            objectName: "usercertLabel"
            fontSize: "medium"
            font.bold: false
            color: Theme.palette.selected.backgroundText
            visible: (securityList.selectedIndex === 2 ||
                      securityList.selectedIndex === 4) &&
                      authList.selectedIndex === 0 // only for TLS

        }

        ListItem.ItemSelector {
            id: usercertSelector
            anchors {
                left: parent.left
                right: parent.right
            }
            visible: (securityList.selectedIndex === 2 ||
                      securityList.selectedIndex === 4) &&
                      authList.selectedIndex === 0 // only for TLS
            model: cacertListModel
            expanded: false
            delegate: certSelectorDelegate
            selectedIndex: 0
            property string usercertFileName: {
                if (selectedIndex !== 0 &&
                    selectedIndex !== (model.rowCount()-1)) {
                      return model.getfileName(selectedIndex);
                } else {
                    return "";
                }
            }
            onSelectedIndexChanged: {
                if (selectedIndex === cacertListModel.rowCount()-1) {
                    selectedIndex = 0;
                    otherNetworkDialog.filePicker(0); //Certificate
                }
            }
        }

        Label {
            id: privatekeyLabel
            text : i18n.tr("User private key")
            objectName: "userprivatekeyLabel"
            fontSize: "medium"
            font.bold: false
            color: Theme.palette.selected.backgroundText
            visible: (securityList.selectedIndex === 2 ||
                      securityList.selectedIndex === 4) &&
                     (authList.selectedIndex === 0) // only for TLS
        }

        PrivatekeyListModel {
            id: privatekeyListModel
        }

        Component{
            id: privatekeySelectorDelegate
            OptionSelectorDelegate {
                text: KeyName

                /* FIXME: Translate using i18n.tr("%1, %1, %1 bit").arg().…
                and add comment to translators about what each argument
                represents. */
                subText: (KeyName !== i18n.tr("None") &&
                          KeyName !== i18n.tr("Choose…")) ?
                          (KeyType + ", " + KeyAlgorithm +", " + KeyLength + " bit" ) :
                          ""
            }
        }

        ListItem.ItemSelector {
            id: privateKeySelector
            anchors {
                left: parent.left
                right: parent.right
            }
            visible: (securityList.selectedIndex === 2 ||
                      securityList.selectedIndex === 4) &&
                     (authList.selectedIndex === 0) // only for TLS
            model: privatekeyListModel
            expanded: false
            delegate: privatekeySelectorDelegate
            selectedIndex: 0
            property string privateKeyFileName: {
                if (selectedIndex !== 0 &&
                    selectedIndex !==
                    (model.rowCount()-1)) {
                    return model.getfileName(
                        selectedIndex
                    );
                } else {
                    return "";
                }
            }
            onSelectedIndexChanged: {
                if (selectedIndex === privatekeyListModel.rowCount()-1) {
                    selectedIndex = 0;
                    otherNetworkDialog.filePicker(1); //Key
                }
            }
        }

        Label {
            id: pacFileLabel
            text : i18n.tr("Pac file")
            objectName: "pacFileLabel"
            fontSize: "medium"
            font.bold: false
            color: Theme.palette.selected.backgroundText
            visible: (securityList.selectedIndex === 2 ||
                      securityList.selectedIndex === 4 /* WPA or D-WEP */) &&
                      (authList.selectedIndex === 3)
        }

        PacFileListModel {
            id: pacFileListModel
        }

        Component{
            id: pacFileSelectorDelegate
            OptionSelectorDelegate { text: pacFileName; }
        }

        ListItem.ItemSelector {
            id: pacFileSelector
            anchors {
                left: parent.left
                right: parent.right
            }
            visible: (securityList.selectedIndex === 2 ||
                      securityList.selectedIndex === 4) &&
                     (authList.selectedIndex === 3) // only for FAST
            model: pacFileListModel
            expanded: false
            delegate: pacFileSelectorDelegate
            selectedIndex: 0
            property string pacFileName: {
                if (selectedIndex !== 0 &&
                    selectedIndex !== (pacFileListModel.rowCount()-1)) {
                       return pacFileListModel.getfileName(selectedIndex);
                } else {
                    return "";
                }
            }
            onSelectedIndexChanged: {
                if (selectedIndex === pacFileListModel.rowCount()-1) {
                    selectedIndex = 0;
                    otherNetworkDialog.filePicker(2); //PacFile
                }
            }
        }

        Label {
            id: pacProvisioningListLabel
            text : i18n.tr("Pac provisioning")
            objectName: "pacProvisioningListLabel"
            fontSize: "medium"
            font.bold: false
            color: Theme.palette.selected.backgroundText
            elide: Text.ElideRight
            visible: (securityList.selectedIndex === 2 ||
                      securityList.selectedIndex === 4) &&
                      (authList.selectedIndex === 3)
        }

        ListItem.ItemSelector {
            id: pacProvisioningList
            objectName: "pacProvisioningList"
            model: [i18n.tr("Disabled"),      // index: 0
                i18n.tr("Anonymous"),         // index: 1
                i18n.tr("Authenticated"),     // index: 2
                i18n.tr("Both"),              // index: 3
            ]
            selectedIndex: 1
            visible: (securityList.selectedIndex === 2 ||
                      securityList.selectedIndex === 4) &&
                     (authList.selectedIndex === 3)
        }

        Label {
            id: anonymousIdentityLabel
            text : i18n.tr("Anonymous identity")
            objectName: "anonymousIdentityLabel"
            fontSize: "medium"
            font.bold: false
            color: Theme.palette.selected.backgroundText
            visible: (securityList.selectedIndex === 2 &&
                      authList.selectedIndex !== 2)
        }

        TextField {
            id : anonymousIdentity
            objectName: "anonymousIdentity"
            width: parent.width
            visible: (securityList.selectedIndex === 2 &&
                      authList.selectedIndex !== 2)
            inputMethodHints: Qt.ImhNoPredictiveText
            onAccepted: {
                connectAction.trigger()
            }
        }

        Label {
            id: usernameLabel
            text : {
                if ((securityList.selectedIndex === 2 ||
                    securityList.selectedIndex === 4) &&
                    (authList.selectedIndex === 0 )) {
                    return i18n.tr("Identity");
                } else {
                    return i18n.tr("Username");
                }
            }
            objectName: "usernameLabel"
            fontSize: "medium"
            font.bold: false
            color: Theme.palette.selected.backgroundText
            elide: Text.ElideRight
            visible: (securityList.selectedIndex === 2 ||
                      securityList.selectedIndex === 4 ||
                      securityList.selectedIndex === 5)
        }

        TextField {
            id : username
            objectName: "username"
            width: parent.width
            visible: (securityList.selectedIndex === 2 ||
                      securityList.selectedIndex === 4 ||
                      securityList.selectedIndex === 5)
            inputMethodHints: Qt.ImhNoPredictiveText
            onAccepted: connectAction.trigger()
        }

        Label {
            id: passwordLabel
            text: {
                if ((securityList.selectedIndex === 2 ||
                     securityList.selectedIndex === 4) &&
                    (authList.selectedIndex === 0)) {
                    return i18n.tr("Private key password");
                } else {
                    return i18n.tr("Password");
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
            onAccepted: connectAction.trigger()
        }

        Row {
            id: passwordVisiblityRow
            layoutDirection: Qt.LeftToRight
            spacing: units.gu(2)
            visible: securityList.selectedIndex !== 0

            CheckBox {
                id: passwordVisibleSwitch
                activeFocusOnPress: false
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
            visible: ((securityList.selectedIndex === 2 ||
                       securityList.selectedIndex === 4) &&
                      (authList.selectedIndex === 1 ||
                       authList.selectedIndex === 3 ||
                       authList.selectedIndex === 4))

            CheckBox {
                id: passwordRememberSwitch
                activeFocusOnPress: false
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
                    [
                        username.text,
                        anonymousIdentity.text
                    ],
                    [
                        password.text,
                        passwordRememberSwitch.checked
                    ],
                    [
                        cacertSelector.cacertFileName,
                        usercertSelector.usercertFileName,
                        privateKeySelector.privateKeyFileName,
                        pacFileSelector.pacFileName,
                        pacProvisioningList.selectedIndex.toString()
                    ],
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
