import QtQuick 2.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem

Column {
    anchors {
        left: parent.left
        right: parent.right
    }
    spacing: units.gu(1)

    property string category
    property string categoryName

    ListItem.Header {
        id: header

        text: categoryName
        visible: repeater.count > 0
    }

    Grid {
        anchors.left: parent.left
        anchors.right: parent.right
        columns: Math.floor(width / units.gu(15))
        columnSpacing: (width - anchors.leftMargin -
                        anchors.rightMargin - (columns * units.gu(15))
                        - parent.spacing * 2) /
                       (columns - 1)

        Repeater {
            id: repeater

            model: pluginManager.itemModel(category)

            delegate: Loader {
                id: loader
                width: units.gu(15)
                sourceComponent: model.item.entryComponent

                Connections {
                    ignoreUnknownSignals: true
                    target: loader.item
                    onClicked: {
                        var pageComponent = model.item.pageComponent
                        if (pageComponent) {
                            pageStack.push(model.item.pageComponent,
                                           { plugin: model.item, pluginManager: pluginManager })
                        }
                    }
                }
            }
        }
    }
    ListItem.ThinDivider { visible: header.visible }
}
