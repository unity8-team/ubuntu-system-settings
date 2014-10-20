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
    title: i18n.tr("Edit Identity")
    property string listindex
    
    MongoDB {
        id: db
        name: "yadaserver"
        host: "localhost"
    }
    WebSocket {
        id: webSocket
        url: "ws://localhost:8901"
        active: true
    }
    Column {
        TextField {
            id: namefield
            text: {
                var collec = db.getCollection('identities')
                var res = collec.find({})
                for (var i=-1; i < parseInt(listindex); i++) {
                    var iden = res.next()
                }
                return iden.data.identity.name
            }
        }
        Button {
            text: "Save"
            onClicked: {
                var collec = db.getCollection('identities')
                var res = collec.find({})
                for (var i=-1; i < parseInt(listindex); i++) {
                    var iden = res.next()
                }
                iden.data.identity.name = namefield.text
                webSocket.sendTextMessage(Json.JSON.stringify({"METHOD": "UPDATE_IDENTITY", "DATA":iden}))
            }
        }
    }
}
