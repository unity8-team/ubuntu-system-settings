import QtQuick 2.0
import QMenuModel 0.1 as QMenuModel
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import Ubuntu.SystemSettings.Wizard.Utils 0.1
import Ubuntu.Settings.Menus 0.1 as Menus
import "../Components" as LocalComponents
import QtMongo 1.0

LocalComponents.Page {
    id: identityPage
    title: i18n.tr("Identity Management")
    forwardButtonSourceComponent: forwardButton

    function getExtendedProperty(object, propertyName, defaultValue) {
        if (object && object.hasOwnProperty(propertyName)) {
            return object[propertyName];
        }
        return defaultValue;
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
	    text: "Add Identity"
	    onClicked: {
                mythings.save({"idlabel":"ID: " + identityInput.text})
                listview.model.update()
	    }
	}
        Label {
            id: label
            anchors.left: parent.left
            anchors.leftMargin: leftMargin
            anchors.right: parent.right
            anchors.rightMargin: rightMargin
            fontSize: "small"
            text: i18n.tr("Your identities")
        }
	ListView {
            id: listview
            focus: true
            highlight: Rectangle { color: "lightsteelblue"; radius: 5 }
            width: 250
            height: 500

            MongoDB {
                id: db
                name: "testdb"
                host: "localhost"

                collections: [
                    MongoCollection {
                            id: mythings
                            name: "things"
                    }
                ]
            }

            // this corresponds to
            // model = mythings.find({x:4}):
            model: MongoQuery {
                collection: mythings
                query: {}
//              if you want to use objects here, you MUST use return stmts:
//              sort: { return { j: -1 } }
            }

            delegate: mydelegate
            Component {
                id: mydelegate
                Text {
                    text: ""+idlabel
                }
            }

            onCurrentIndexChanged: jsonEdit.object = listview.model.get(listview.currentIndex)
        }
    }
    Component {
        id: forwardButton
        LocalComponents.StackButton {
            text: i18n.tr("Continue")
            rightArrow: true
            onClicked: pageStack.next()
        }
    }
}
