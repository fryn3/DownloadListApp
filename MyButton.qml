import QtQuick 2.15
import QtQuick.Controls 2.15

AbstractButton {
    id: root

    property alias radius: bg.radius
    property alias borderWidth: bg.border.width
    property alias colorEnable: _c.enable
    property alias colorDown: _c.down
    property alias colorHover: _c.hover
    property alias colorDisable: _c.disable

    text: "Button"
    display: AbstractButton.TextBesideIcon
    leftPadding: width > implicitContentWidth ? (width - implicitContentWidth) / 2 : 0
    rightPadding: width > implicitContentWidth ? (width - implicitContentWidth) / 2 : 0
    background: Rectangle {
        id: bg
        color: root.enabled
               ? (root.down
                  ? _c.down
                  : (root.hovered ? _c.hover : _c.enable)) : _c.disable
        border {
            width: 0
            color: root.hovered ? "#0078CE" : root.activeFocus ? "#FFFFFF" : "#566976"
        }
    }
    contentItem: Row {
        anchors.centerIn: root
        height: Math.max(ico.height, txt.height)
        leftPadding: 10
        rightPadding: leftPadding
        topPadding: 10
        bottomPadding: topPadding
        spacing: 8
        Image {
            id: ico
            anchors.verticalCenter: parent.verticalCenter
            width: root.icon.width
            height: root.icon.height
            source: root.icon.source
            opacity: root.enabled ? 1 : 0.5
        }
        Label {
            id: txt
            anchors.verticalCenter: parent.verticalCenter
            text: root.text
            verticalAlignment: Text.AlignVCenter
            color: "#FFFFFF"
            font: root.font
            opacity: root.enabled ? 1 : 0.5
        }
    }

    QtObject {
        id: _c
        property color enable: "#0078CE"
        property color down: "#026EBB"
        property color hover: "#59B3F4"
        property color disable: "#7D92A1"
    }
}
