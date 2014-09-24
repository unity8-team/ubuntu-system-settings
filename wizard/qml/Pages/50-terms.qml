/*
 * Copyright (C) 2013 Canonical, Ltd.
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

import QtQuick 2.3
import QMenuModel 0.1
import Qt.labs.folderlistmodel 2.1
import Ubuntu.Components 1.1
import Ubuntu.SystemSettings.Wizard.Utils 0.1
import "../Components" as LocalComponents

LocalComponents.Page {
    title: i18n.tr("Terms")
    forwardButtonSourceComponent: forwardButton

    property bool pathSet: System.hereLicensePath !== " " // single space means it's unassigned
    property bool countSet: false
    skipValid: pathSet && (System.hereLicensePath === "" || countSet)
    skip: skipValid && (System.hereLicensePath === "" || termsModel.count === 0)

    Connections {
        target: termsModel
        onCountChanged: if (pathSet) countSet = true
    }

    FolderListModel {
        id: termsModel
        folder: System.hereLicensePath
        nameFilters: ["*.html"]
        showDirs: false
        showOnlyReadable: true
    }

    Column {
        id: column
        anchors.fill: content
        spacing: units.gu(4)

        Item { // spacer
            height: units.gu(1)
            width: units.gu(1) // needed else it will be ignored
        }

        LocalComponents.CheckableSetting {
            id: termsCheck
            showDivider: false
            text: i18n.tr("I have read and agreed to the HERE <a href='here-terms.qml'>terms and conditions</a>")
            onLinkActivated: pageStack.load(Qt.resolvedUrl(link))
            checked: System.hereEnabled
            onTriggered: System.hereEnabled = checked
        }
    }

    Component {
        id: forwardButton
        LocalComponents.StackButton {
            text: i18n.tr("Continue")
            onClicked: pageStack.next()
        }
    }
}
