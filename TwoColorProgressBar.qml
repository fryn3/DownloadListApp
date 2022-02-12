import QtQuick 2.15

Rectangle {
    id: root
    property real from: 0
    property real to: 1
    property real value: 0

    property alias progressColor: rect.color

    Item {
        height: parent.height
        width: from >= to ? parent.width
                          : parent.width * (value - from) / (to - from)
        clip: true
        Rectangle {
            id: rect
            width: root.width
            height: root.height
            radius: root.radius
        }
    }
}
