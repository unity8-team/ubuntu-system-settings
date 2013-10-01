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
        property int itemWidth: units.gu(15)

        anchors.leftMargin: (parent.width - width) / 2
        anchors.rightMargin: anchors.leftMargin
        width: (columns * itemWidth) + columnSpacing * (columns - 1)
        anchors.horizontalCenter: parent.horizontalCenter
        columns: Math.floor(parent.width / itemWidth)
        columnSpacing: spacing

        Repeater {
            id: repeater

            model: pluginManager.itemModel(category)

            delegate: Loader {
                id: loader
                width: parent.itemWidth
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
