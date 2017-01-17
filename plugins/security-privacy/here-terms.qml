/*
 * Copyright (C) 2014 Canonical, Ltd.
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

import QtQuick 2.4
import Qt.labs.folderlistmodel 2.1
import SystemSettings 1.0
import Ubuntu.Components 1.3
import Ubuntu.SystemSettings.SecurityPrivacy 1.0

ItemPage {
    title: i18n.tr("Nokia HERE")
    flickable: scrollWidget
    id: termsPage

    UbuntuSecurityPrivacyPanel {
        id: securityPrivacy
    }

    FolderListModel {
        id: termsModel
        folder: securityPrivacy.hereLicensePath
        nameFilters: ["*.html"]
        showDirs: false
        showOnlyReadable: true
        onCountChanged: loadFileContent()
    }

    function makeFileName(lang, country) {
        return lang + "_" + country + ".html"
    }

    function defaultCountryForLanguage(lang) {
        if (lang === "da") return "DK"
        if (lang === "en") return "US"
        if (lang === "ko") return "KR"
        if (lang === "zh") return "CN"
        return lang.toUpperCase()
    }

    function determineFileName() {
        var codes = i18n.language.split(".")[0].split("_")
        var defaultCountry = defaultCountryForLanguage(codes[0])
        if (codes.count === 1)
            codes = [codes[0], defaultCountry]
        var perfectMatch = makeFileName(codes[0], codes[1])
        var nearMatch = makeFileName(codes[0], defaultCountry)
        var nearMatchExists = false

        for (var i = 0; i < termsModel.count; i++) {
            var fileName = termsModel.get(i, "fileName")
            if (fileName == perfectMatch) {
                return perfectMatch
            } else if (fileName == nearMatch) {
                nearMatchExists = true
            }
        }

        if (nearMatchExists) {
            return nearMatch
        } else {
            return makeFileName("en", "US")
        }
    }

    function loadFileContent() {
        var xhr = new XMLHttpRequest
        xhr.open("GET", securityPrivacy.hereLicensePath + "/" + determineFileName())
        console.warn('opening', securityPrivacy.hereLicensePath + "/" + determineFileName())
        xhr.onreadystatechange = function() {
            if (xhr.readyState == XMLHttpRequest.DONE) {
                termsLabel.text = xhr.responseText
            }
        }
        xhr.send()
    }
    Flickable {
        id: scrollWidget
        anchors.fill: parent
        contentHeight: contentItem.childrenRect.height
        boundsBehavior: (contentHeight > termsPage.height) ? Flickable.DragAndOvershootBounds : Flickable.StopAtBounds
        /* Set the direction to workaround https://bugreports.qt-project.org/browse/QTBUG-31905
           otherwise the UI might end up in a situation where scrolling doesn't work */
        flickableDirection: Flickable.VerticalFlick

        Column {
            width: scrollWidget.width

            Item {
                height: units.gu(2)
                anchors {
                    left: parent.left
                    right: parent.right
                }
            }

            Label {
                id: termsLabel
                anchors {
                    margins: units.gu(2)
                    left: parent.left
                    right: parent.right
                }
                wrapMode: Text.Wrap
                linkColor: Theme.palette.normal.backgroundText
                onLinkActivated: {
                    Qt.openUrlExternally(link)
                }
            }
        }
    }
}
