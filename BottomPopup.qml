import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Popup {
    parent: Overlay.overlay
    focus: true
    width: parent.width > 600 ? 590 : parent.width - 10
    x: Math.round((parent.width - width) / 2)
    y: parent.height - height - menu_bottom.height

    background: Rectangle {
        color:"transparent"
        Rectangle {
            width: 1
            anchors.fill: parent
            radius: 10
            color: Material.background
            opacity: 0.9
        }
    }

}
