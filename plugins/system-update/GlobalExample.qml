import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.SystemSettings.Update 1.0
        // Idle,
        // CheckingClickUpdates,
        // CheckingSystemUpdates,
        // CheckingAllUpdates,
        // NetworkError,
        // ServerError

Column {
    width: units.gu(50)
    height: units.gu(90)

    Global {
        id: glob
        anchors {
            left: parent.left;
            right: parent.right
            margins: units.gu(2)
        }

        managerStatus: UpdateManager.CheckingSystemUpdates
        requireRestart: false
        updatesCount: 0
        online: true

        height: hidden ? 0 : units.gu(6)
    }

    Column {
        anchors { left: parent.left; right: parent.right }

        Rectangle {
            anchors { left: parent.left; right: parent.right }
            height: units.gu(2)
            color: "red"
        }
    }

    Button {
        text: "Check"
        onClicked: {
            glob.updatesCount = 0
            glob.managerStatus = UpdateManager.CheckingAllUpdates
        }
    }

    Button {
        text: "Make irrelevant"
        onClicked: {
            glob.updatesCount = 1
            glob.managerStatus = UpdateManager.Idle
        }
    }

    Button {
        text: "Queue system install"
        onClicked: {
            glob.updatesCount = 2
            glob.requireRestart = true;
            glob.managerStatus = UpdateManager.Idle
        }
    }

    Button {
        text: "Queue click update"
        onClicked: {
            glob.updatesCount = 12
            glob.requireRestart = false;
            glob.managerStatus = UpdateManager.Idle
        }
    }
}
