import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQml 2.15

Item {
    id: root

    readonly property string stateNone: "StateNone"
    readonly property string stateDownloading: "StateDownloading"
    property alias login: tfLogin.text
    property alias password: tfPass.text
    property alias saved: checkSave.checked

    implicitWidth: 286
    implicitHeight: 410

    state: stateNone

    Rectangle {
        width: 1
        height: parent.height
        color: "#3A4349"
    }

    Column {
        anchors {
            fill: parent
            topMargin: 32
            leftMargin: 24
            rightMargin: anchors.leftMargin
        }

        spacing: 12

        MyTextField {
            id: tfLogin

            anchors {
                left: parent.left
                right: parent.right
            }
            height: 48
            radius: 8
            borderWidth: 1

            placeholderText: "Логин"
        }

        MyTextField {
            id: tfPass

            anchors {
                left: parent.left
                right: parent.right
            }
            height: 48
            radius: 8
            borderWidth: 1

            placeholderText: "Пароль"
            echoMode: TextInput.Password
        }

        CheckBox {
            id: checkSave

            text: "Запомнить"
            font.pixelSize: 12

            indicator: Rectangle {
                x: 8//checkSave.leftPadding
                y: parent.height / 2 - height / 2
                implicitWidth: 16
                implicitHeight: 16

                color: "transparent"
                radius: 4
                border {
                    width: 1
                    color: checkSave.hovered ? "#0078CE" : checkSave.activeFocus ? "#FFFFFF" : "#566976"
                }

                Image {
                    anchors.fill: parent
                    source: "qrc:/icons/check.svg"
                    visible: checkSave.checked
                }
            }

            contentItem: Text {
                text: checkSave.text
                font: checkSave.font
                color: checkSave.hovered ? "#0078CE" : checkSave.activeFocus ? "#FFFFFF" : "#566976"
                verticalAlignment: Text.AlignVCenter
                leftPadding: checkSave.indicator.width + checkSave.spacing
            }
        }
    }

    states: [
        State {
            name: stateNone
        },
        State {
            name: stateDownloading
            PropertyChanges {
                target: tfLogin
                enabled: false
            }
            PropertyChanges {
                target: tfPass
                enabled: false
            }
        }
    ]}
