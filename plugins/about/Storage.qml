import QtQuick 2.0
import QtQuick.XmlListModel 2.0
import QtSystemInfo 5.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import SystemSettings 1.0

ItemPage {
    id: storagePage

    title: i18n.tr("Storage")
    flickable: scrollWidget
    property var spaceColors: ["white", UbuntuColors.orange, UbuntuColors.lightAubergine]
    property var spaceLabels: [i18n.tr("Free space"), i18n.tr("Used by Ubuntu"),
        i18n.tr("Used by apps")]
    property var spaceValues: ["31.4 GB", "19.6 GB", "13.0 GB"]
    property bool sortByName: true

    /* TOFIX: replace by real datas */
    XmlListModel {
        id: xmlModel
        source: "fakepkgslist.xml"
        query: "/list/binary"

        XmlRole { name: "binaryName"; query: "name/string()" }
        XmlRole { name: "iconName"; query: "icon/string()" }
        XmlRole { name: "installedSize"; query: "installed/string()" }

        onStatusChanged: if (status === XmlListModel.Ready) { createSortedLists(); }
    }

    ListModel {
        id: sortedNamesModel
    }

    ListModel {
        id: sortedInstallModel
    }

    function createSortedLists() {
        var n;
        var namesDict = {};
        var nameKeys = [];
        var installDict = {};
        var installKeys = [];

        for (n=0; n < xmlModel.count; n++) {
            namesDict[xmlModel.get(n).binaryName] = [xmlModel.get(n).iconName, xmlModel.get(n).installedSize];
            installDict[xmlModel.get(n).installedSize] = [xmlModel.get(n).iconName, xmlModel.get(n).binaryName];
        }

        nameKeys = Object.keys(namesDict).sort();
        installKeys = Object.keys(installDict).sort(function(a,b){return b-a});

        for (n=0; n < nameKeys.length; n++) {
            sortedNamesModel.append({"binaryName":nameKeys[n],"iconName":namesDict[nameKeys[n]][0],"installedSize":namesDict[nameKeys[n]][1]})
            sortedInstallModel.append({"binaryName":installDict[installKeys[n]][1],"iconName":installDict[installKeys[n]][0],"installedSize":installKeys[n]})
        }
    }

    /* Return used space in a formatted way */
    function getFormattedSpace(space) {
        if (space < 1000)
            return space;
        if (space / 1000 < 1000)
            return Math.round(space / 1000) + " " + i18n.tr("kB");
        else if (space/1000/1000 < 1000)
            return Math.round((space / 1000 / 1000) * 10) / 10 + " " + i18n.tr("MB");
        else if (space/1000/1000/1000 < 1000)
            return Math.round((space / 1000 / 1000 / 1000) * 10) / 10 + " " + i18n.tr("GB");
        return "";
    }

    StorageInfo {
        id: storageInfo
    }

    Flickable {
        id: scrollWidget
        anchors.fill: parent
        contentHeight: columnId.height

        Column {
            id: columnId
            anchors.left: parent.left
            anchors.right: parent.right

            ListItem.SingleValue {
                id: diskItem
                text: i18n.tr("Total storage")
                value: storagePage.getFormattedSpace(storageInfo.totalDiskSpace('/'));
                showDivider: false
            }

            StorageBar {
                colors: spaceColors
            }

            Repeater {
                model: spaceColors
                Item {
                    height: units.gu(3)
                    width: parent.width-units.gu(4)
                    anchors.horizontalCenter: parent.horizontalCenter
                    Row {
                        spacing: units.gu(1)

                        Rectangle {
                            width: units.gu(2)
                            height: units.gu(2)
                            border.width: units.dp(1)
                            color: modelData
                        }
                        Label { text: spaceLabels[index] }
                    }
                    Label {
                        anchors.right: parent.right
                        text: spaceValues[index]
                    }
                }
            }

            ListItem.ValueSelector {
                id: valueSelect
                values: [i18n.tr("By name"), i18n.tr("By size")]
            }

            ListView {
                anchors.left: parent.left
                anchors.right: parent.right
                height: childrenRect.height
                /* Desactivate the listview flicking, we want to scroll on the column */
                interactive: false
                model: (valueSelect.selectedIndex === 0) ? sortedNamesModel : sortedInstallModel
                delegate: ListItem.SingleValue {
                    icon: "image://gicon/" + iconName
                    fallbackIconSource: "image://gicon/clear"   // TOFIX: use proper fallback
                    text: binaryName
                    value: storagePage.getFormattedSpace(installedSize)
                }
            }
        }
    }
}
