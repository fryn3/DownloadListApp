import QtQuick 2.15
import QtQuick.Controls 2.15
import Qt.labs.settings 1.1
import QtQml 2.15

Item {
    id: root

    property var downloadModel: core ? core.model : null

    Connections {
        target: core
        function onFinishedDownload() {
            root.state = p.stateSelected;
        }
    }

    QtObject {
        id: p

        readonly property string stateNone: "StateNone"
        readonly property string stateSelected: "StateSelected"
        readonly property string stateDownloading: "StateDownloading"
    }

    Settings {
        id: sFiealds
        category: "fields"
        property alias login: auth.login
        property string pass: ""
        property alias checkPass: auth.saved
        property alias pathSource: controls.pathSource
        property alias pathDest: controls.pathDestination
    }

    Component.onDestruction: {
        if (auth.saved) {
            sFiealds.pass = auth.password;
        } else {
            sFiealds.pass = ""
        }
    }
    Component.onCompleted: {
        auth.password = sFiealds.pass;
    }

    ListView {
        id: listView

        anchors {
            top: parent.top
            topMargin: 32
            bottom: controls.top
            bottomMargin: 32
            left: parent.left
            leftMargin: 24
            right: auth.left
            rightMargin: 24
        }

        clip: true
        spacing: 4
        model: downloadModel
        delegate: DeligateDownload {
            width: listView.width
            num: index + 1
            stateRow: downloadState
            text: downloadUrl
            value: downloadBytesReceived
            to: downloadBytesTotal
        }
        ScrollBar.vertical: ScrollBar { }
        add: Transition {
            NumberAnimation {
                properties: "x";
                from: -200
                duration: 1000
                easing.type: Easing.OutElastic
                easing.amplitude: .5
            }
        }
    }

    AuthorizationItem {
        id: auth

        anchors {
            top: parent.top
            bottom: controls.top
            right: parent.right
        }
        width: 286

        onLoginChanged: {
            core.login = login;
        }
        onPasswordChanged: {
            core.pass = password;
        }
        onSavedChanged: {
            core.saveLogin = saved;
        }
    }

    ControlsItem {
        id: controls

        anchors {
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }
        height: 222
        progress.value: core.nextTryDownload
        progress.to: downloadModel.rowCount + 1

        onPathSourceChanged: {
            core.sourcePath = pathSource;
        }

        onPathDestinationChanged: {
            core.destinationPath = pathDestination;
        }

        onBtnDownloadClicked: {
            console.assert(root.state !== p.stateNone);
            if (root.state === p.stateSelected) {
                root.state = p.stateDownloading;
                core.startDownload();
            } else if (root.state === p.stateDownloading) {
                root.state = p.stateSelected;
                core.stopDownload();
            }
        }
    }

    states: [
        State {
            name: p.stateNone
        },
        State {
            name: p.stateSelected
            when: downloadModel.rowCount > 0 && controls.pathDestination.length > 0
            PropertyChanges {
                target: controls
                state: controls.stateSelected
            }
        },
        State {
            name: p.stateDownloading
            PropertyChanges {
                target: controls
                state: controls.stateDownloading
            }
            PropertyChanges {
                target: auth
                state: auth.stateDownloading
            }
        }
    ]
}
