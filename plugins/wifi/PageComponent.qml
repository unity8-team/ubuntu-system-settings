import QtQuick 2.0
import SystemSettings 1.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import Ubuntu.SystemSettings.Phone 1.0
import QMenuModel 0.1


IndicatorBase {
    title: i18n.tr("Wi-Fi")
	busName: "com.canonical.indicator.network"
	actionsObjectPath: "/com/canonical/indicator/network"
	menuObjectPath: {"phone_wifi_settings": "/com/canonical/indicator/network/phone_wifi_settings"}

    MenuItemFactory {
        id: menuFactory
		model: menuModel
    }

    ListView {
        id: mainMenu
        model: menuModel

        anchors {
            fill: parent
            bottomMargin: Qt.inputMethod.visible ? (Qt.inputMethod.keyboardRectangle.height - main.anchors.bottomMargin) : 0

            Behavior on bottomMargin {
                NumberAnimation {
                    duration: 175
                    easing.type: Easing.OutQuad
                }
            }
            // TODO - does ever frame.
            onBottomMarginChanged: {
                mainMenu.positionViewAtIndex(mainMenu.currentIndex, ListView.End)
            }
        }

        // Ensure all delegates are cached in order to improve smoothness of scrolling
        cacheBuffer: 10000

        // Only allow flicking if the content doesn't fit on the page
        interactive: contentHeight > height

        currentIndex: -1
        delegate: Item {
            id: menuDelegate

            anchors {
                left: parent.left
                right: parent.right
            }
            height: loader.height
            visible: height > 0

            Loader {
                id: loader
                asynchronous: true

                property int modelIndex: index

                anchors {
                    left: parent.left
                    right: parent.right
                }

                sourceComponent: menuFactory.load(model)

                onLoaded: {
                    if (model.type === rootMenuType) {
                        menuStack.push(mainMenu.model.submenu(index));
                    }

                    if (item.hasOwnProperty("menuActivated")) {
                        item.menuActivated = Qt.binding(function() { return ListView.isCurrentItem; });
                        item.selectMenu.connect(function() { ListView.view.currentIndex = index });
                        item.deselectMenu.connect(function() { ListView.view.currentIndex = -1 });
                    }
                    if (item.hasOwnProperty("menu")) {
                        item.menu = Qt.binding(function() { return model; });
                    }
                }

                Binding {
                    target: item ? item : null
                    property: "objectName"
                    value: model.action
                }
            }
        }
    }
}
