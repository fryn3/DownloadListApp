import QtQuick 2.15
import QtQuick.Controls 2.15
import QtGraphicalEffects 1.13
import QtQuick.Layouts 1.15

Control {
    property alias textField: t
    property alias button: b
    implicitWidth: 500
    implicitHeight: 48
    Control {
        id: rc
        anchors {
            fill: parent
            margins: borderRect.border.width
            centerIn: parent
        }
        layer.enabled: true
        layer.effect: OpacityMask {
            maskSource: Rectangle {
                width: rc.width
                height: rc.height
                radius: borderRect.radius - borderRect.border.width - 1
                border {
                    width: borderRect.border.width
                    color: "transparent"
                }
            }
        }
        MyTextField {
            id: t
            anchors {
                top: parent.top
                bottom: parent.bottom
                left: parent.left
                right: b.left
            }
        }
        MyButton {
            id: b
            anchors {
                top: parent.top
                bottom: parent.bottom
                right: parent.right
            }
            width: 224
        }
    }
    background: Rectangle {
        id: borderRect
        color: "transparent"
        radius: 8
        border {
            width: 1
            color: t.hovered || b.hovered ? "#0078CE" : t.activeFocus || b.activeFocus ? "#FFFFFF" : "#566976"
        }
    }
}
