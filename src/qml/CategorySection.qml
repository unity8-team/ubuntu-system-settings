import QtQuick 2.4
import SystemSettings 1.0
import SystemSettings.ListItems 1.0 as SettingsListItems
import Ubuntu.Components 1.3


Column {
    anchors {
        left: parent.left
        right: parent.right
    }
    spacing: units.gu(1)

    property string category
    property string categoryName

    objectName: "categoryGrid-" + category

    SettingsItemTitle {
        id: header
        text: categoryName
        visible: repeater.count > 0
    }

    Column {
        id: col
        anchors {
            left: parent.left
            right: parent.right
        }

        Repeater {
            id: repeater

            model: pluginManager.itemModel(category)

            delegate: Loader {
                id: loader
                anchors {
                    left: col.left
                    right: col.right
                }
                sourceComponent: model.item.entryComponent
                active: model.item.visible
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
}
