import QtQuick 2.15
import QtQuick.Controls 2.15

TextField {
    id: root
    property alias radius: bg.radius
    property alias borderWidth: bg.border.width
    property alias colorTextEnable: _c.textEnable
    property alias colorTextDisable: _c.textDisable
    property alias colorTextReadOnly: _c.textReadOnly
    placeholderTextColor: "#566976"

    placeholderText: qsTr("Text Field")
    color: enabled ? (readOnly ? colorTextReadOnly : colorTextEnable) : colorTextDisable
    leftPadding: 10
    rightPadding: leftPadding

    background: Rectangle {
        id: bg
        border {
            width: 0
            color: root.hovered ? "#0078CE" : root.activeFocus ? "#FFFFFF" : "#566976"
        }
        color: !root.enabled ? "#566976" : "#111B22"
    }
    font {
        pixelSize: 16
    }
    onActiveFocusChanged: {
        if (readOnly && activeFocus) {
            selectAll();
        }
    }

    QtObject {
        id: _c
        property color textEnable: "#FFFFFF"
        property color textDisable: "#222222"
        property color textReadOnly: "#CCCCCC"
    }
}
