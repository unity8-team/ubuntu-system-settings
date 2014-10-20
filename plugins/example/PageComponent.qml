import QtQuick 2.0
import QMenuModel 0.1 as QMenuModel
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import Ubuntu.SystemSettings.Wizard.Utils 0.1
import Ubuntu.Settings.Menus 0.1 as Menus
import QtMongo 1.0
import SystemSettings 1.0
import "file:///home/phablet/identity-management/plugins/example/Components" as LocalComponents
import "file:///usr/lib/arm-linux-gnueabihf/qt5/imports/QtMongo/lib/json/json2.js" as Json
import Qt.WebSockets 1.0

ItemPage {
    id: identityPage
    title: i18n.tr("Identity Management")

    function getExtendedProperty(object, propertyName, defaultValue) {
        if (object && object.hasOwnProperty(propertyName)) {
            return object[propertyName];
        }
        return defaultValue;
    }
    WebSocket {
        id: webSocket
        url: "ws://localhost:8901"
        onTextMessageReceived: {
            messageBox.text = messageBox.text + "\nReceived secure message: " + message
        }
        active: true
    }

    Column {
        id: column
        spacing: units.gu(2)
        anchors.top: content.top
        anchors.bottom: content.bottom
        anchors.left: identityPage.left
        anchors.right: identityPage.right

        TextField {
            id: identityInput
            placeholderText: "Name for identity eg. Work"
            width: parent.width
        }
        Button {
            id: addid
            text: "Add Identity"
            onClicked: { 
                var inputText = identityInput.text.replace(/\W/g, '')
                if (inputText != "") {
                    webSocket.sendTextMessage(Json.JSON.stringify({"METHOD": "CREATE_IDENTITY", "DATA": inputText}));
                    identityInput.text = "";
                }
            }
        }
        Label {
            id: label
            anchors.left: parent.left
            anchors.right: parent.right
            fontSize: "small"
            text: i18n.tr("Your identities")
        }
        ListView {
            id: listview
            focus: true
            width: 250
            height: 500

            MongoDB {
                id: db
                name: "yadaserver"
                host: "localhost"

                collections: [
                    MongoCollection {
                            id: identities
                            name: "identities"
                    }
                ]
            }

            // this corresponds to
            // model = mythings.find({x:4}):
            model: MongoQuery {
                collection: identities
                query: {}
//                if you want to use objects here, you MUST use return stmts:
//                sort: { return { j: -1 } }
            }
            delegate: mydelegate
	    Component {
		id: mydelegate
	        Text {
	            text: idlabel 
	            font.pointSize: 40
                    MouseArea {
                        id: mouse_area1
                        z: 1
                        hoverEnabled: false
                        anchors.fill: parent
                        onClicked: pageStack.push(Qt.resolvedUrl("profile.qml"), {listindex: index})
                    }
	        }
	    }
            Timer {
                onTriggered: listview.model.update()
                triggeredOnStart: true
                repeat: true
                running: true
            }
            onCurrentIndexChanged: jsonEdit.object = listview.model.get(listview.currentIndex)
        }
    }
}
