import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Dialogs 1.3
import QtGraphicalEffects 1.13
import QtQml 2.15

Item {
    id: root

    readonly property string stateNone: "StateNone"
    readonly property string stateSelected: "StateSelected"
    readonly property string stateDownloading: "StateDownloading"

    property alias pathSource: selectSource.textField.text
    property alias pathDestination: selectDir.textField.text
    property alias progress: _progress

    signal btnDownloadClicked()

    implicitWidth: 224 * 2 + 10
    implicitHeight: col.implicitHeight

    state: stateNone

    FileDialog {
        id: fileDialog
        title: "Please choose a file"

        property var callback: null

        folder: shortcuts.home
        onAccepted: {
            if (callback) {
                callback();
            }
        }
    }

    Rectangle {
        width: parent.width
        height: 1

        color: "#3A4349"
    }

    Column {
        id: col

        anchors {
            fill: parent
            topMargin: 25
            leftMargin: 24
            rightMargin: 24
            bottomMargin: 32
        }

        spacing: 16

        readonly property string _prefix: Qt.platform.os === "windows" ? 'file:///' : 'file://'

        LineAndButton {
            id: selectSource

            width: parent.width

            textField {
                placeholderText: "Путь к файлу для чтения"
                readOnly: true
            }
            button {
                text: "Выбрать файл-источник"
                onClicked: {
                    fileDialog.callback = () => {
                        const s = (fileDialog.fileUrl + '').startsWith(col._prefix)
                                ? (fileDialog.fileUrl + '').slice(col._prefix.length)
                                : fileDialog.fileUrl + '';
                        selectSource.textField.text = s;
                    };
                    fileDialog.title = button.text;
                    fileDialog.selectFolder = false;
                    fileDialog.open();
                }
            }
        }
        LineAndButton {
            id: selectDir

            width: parent.width

            button.text: "Выбрать папку сохранения"
            textField {
                placeholderText: "Путь к папке сохранения"
                readOnly: true
            }
            button.onClicked: {
                fileDialog.callback = () => {
                    const s = (fileDialog.fileUrl + '').startsWith(col._prefix)
                    ? (fileDialog.fileUrl + '').slice(col._prefix.length)
                    : fileDialog.fileUrl + '';
                    selectDir.textField.text = s;
                };
                fileDialog.title = button.text;
                fileDialog.selectFolder = true;
                fileDialog.open();
            }
        }

        RowLayout {
            width: parent.width
            height: 40

            spacing: 24

            ProgressBar {
                id: _progress

                Layout.fillWidth: true
                height: 40

                value: 0
                padding: 2
                background: Rectangle {
                    id: rrr

                    implicitWidth: 200
                    implicitHeight: 32

                    color: "transparent"
                    radius: 24
                    border {
                        width: 1
                        color: "#566976"
                    }
                }
                contentItem: Item {
                    id: contIII

                    anchors {
                        fill: parent
                        margins: rrr.border.width
                        centerIn: parent
                    }
                    layer.enabled: true
                    layer.effect: OpacityMask {
                        maskSource: Rectangle {
                            width: contIII.width
                            height: contIII.height
                            radius: rrr.radius
                        }
                    }
                    Rectangle {
                        width: _progress.visualPosition * contIII.width
                        height: contIII.height

                        radius: 24
                        color: _progress.enabled ? "#0078CE" : "#7D92A1"
                    }

                    Text {
                        id: ttt

                        anchors.centerIn: parent

                        visible: implicitWidth < parent.width
                        text: "%1%".arg(Math.round(_progress.visualPosition * 100))
                        color: "#FFFFFF"
                        font.pixelSize: 16
                    }
                }

                Behavior on value {
                    NumberAnimation { duration: 200 }
                }
            }

            MyButton {
                id: d

                width: 224
                height: 40

                enabled: false
                radius: 4
                borderWidth: 1

                colorEnable: "#31AAB7"
                colorHover: "#6DCDD8"
                colorDown: "#129AA6"
                text: "Начать загрузку"
                icon {
                    source: "qrc:/icons/download.svg"
                    width: 24
                    height: 24
                }
                onClicked: {
                    root.btnDownloadClicked();
                }
            }
        }
    }

    states: [
        State {
            name: stateNone
        },
        State {
            name: stateSelected
            PropertyChanges {
                target: d
                enabled: true
            }
        },
        State {
            name: stateDownloading
            PropertyChanges {
                target: d
                enabled: true
                colorEnable: "#CD9E57"
                colorHover: "#E4B773"
                colorDown: "#CE9B4F"
                text: "Остановить загрузку"
                icon.source: "qrc:/icons/stop.svg"
            }
            PropertyChanges {
                target: selectSource
                button.enabled: false
            }
            PropertyChanges {
                target: selectDir
                button.enabled: false
            }
        }

    ]

}
