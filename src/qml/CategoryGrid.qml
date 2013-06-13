import QtQuick 2.0
import Ubuntu.Components 0.1

Column {
    anchors {
        left: parent.left
        right: parent.right
        leftMargin: units.gu(1)
    }
    spacing: units.gu(1)

    property string category

    Label {
        text: category.toUpperCase()
    }
    Grid {
        anchors.left: parent.left
        anchors.right: parent.right
        columns: width / units.gu(14)

        Repeater {
            model: pluginManager.itemModel(category)

            delegate: Loader {
                id: loader
                width: units.gu(14)
                sourceComponent: model.item.entryComponent

                Connections {
                    ignoreUnknownSignals: true
                    target: loader.item
                    onClicked: pageStack.push(model.item.pageComponent,
                                              { plugin: model.item })
                }
            }
        }
    }
}
