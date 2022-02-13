import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import cpp.DownloadListModel 12.34

TwoColorProgressBar {
    id: root

    property int num
    property int stateRow: DownloadListModel.StateNone
    property string text: "value"
    property real bytesReceived: 0
    property real bytesTotal: 1

    implicitWidth:  730
    implicitHeight: 56

    radius: 4
    color: "#2B3B47"
    progressColor: "#414D4C"
    value: bytesReceived
    to: bytesTotal

    Behavior on value {
        NumberAnimation {
            duration: 200
        }
    }

    Popup {
        id: popup
        anchors.centerIn: parent
        implicitWidth: popupText.implicitWidth + 2 * padding
        implicitHeight: popupText.implicitHeight + 2 * padding
        visible: opacity > 0
        opacity: 0
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent

        background: Rectangle {
            color: "#7D92A1"
            radius: 8
        }
        contentItem: Text {
            id: popupText
            text: "Строка скопирована"
        }

        onOpened: {
            popupTimer.start()
        }

        Behavior on opacity {
            NumberAnimation { duration: 150 }
        }

        Timer {
            id: popupTimer
            interval: 1000
            onTriggered: {
                popup.opacity = 0;
            }
        }
    }

    RowLayout {
        id: row
        anchors {
            left: parent.left
            leftMargin: 16
            verticalCenter: parent.verticalCenter
            right: parent.right
            rightMargin: 16
        }
        spacing: 8

        Text {
            width: 30
            Layout.alignment: Qt.AlignVCenter
            Layout.minimumWidth: width

            text: root.num
            color: "white"
            horizontalAlignment: Text.AlignRight
        }

        Image {
            id: icon
            width: 40
            height: width
            Layout.minimumWidth: width
            Layout.alignment: Qt.AlignVCenter
            source: {
                switch (stateRow) {
                case DownloadListModel.StateNone:
                    return "qrc:/icons/stateNone.svg";
                case DownloadListModel.StateProcess:
                    return"qrc:/icons/stateDownloading.svg";
                case DownloadListModel.StateDone:
                    return"qrc:/icons/stateDownloaded.svg";
                case DownloadListModel.StateError:
                    return"qrc:/icons/stateError.svg";
                }
            }

            ToolTip.visible: row.implicitWidth > root.width && ma.containsMouse
            ToolTip.text: root.text
            ToolTip.delay: 2000
        }

        TextInput {
            id: textInput
            Layout.alignment: Qt.AlignVCenter
            Layout.fillWidth: true

            text: root.text
            color: "white"
            readOnly: true
            clip: true
            cursorPosition: text.length
        }
    }
    MouseArea {
        id: ma
        anchors.fill: parent
        hoverEnabled: true
        onClicked: {
            textInput.selectAll();
            textInput.copy();
            textInput.cursorPosition = textInput.text.length;
            popup.opacity = 1
        }
    }

    states: [
        State {
            when: stateRow === DownloadListModel.StateNone
        },
        State {
            when: stateRow === DownloadListModel.StateProcess
            PropertyChanges {
                target: root
                color: "#2B3B47"
                progressColor: "#414D4C"
                value: bytesReceived
                to: bytesTotal
            }
        },
        State {
            when: stateRow === DownloadListModel.StateDone
            PropertyChanges {
                target: root
                value: to
            }
        },
        State {
            when: stateRow === DownloadListModel.StateError
            PropertyChanges {
                target: root
                color: "#3F4248"
                progressColor: "#3F4248"
                value: 0
            }
        }
    ]
}
