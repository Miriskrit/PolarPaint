import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Popup {
    id: popup
    property var cvs: null

    parent: Overlay.overlay
    focus: true
    width: parent.width - 10;

    x: Math.round((parent.width - width) / 2)
    y: parent.height - height - bottomMenu.height

    ColumnLayout {
        width: parent.width
        x: 0
        y: parent.height - height


        RowLayout {
            width: parent.width


            Label {
                text: qsTr("symmetry: ") + cvs.symmetry
            }

            Switch {
                onClicked: {
                    cvs.symmetry = !cvs.symmetry
                }
            }
        }

        Label {
            text: qsTr("number of axes: ") + cvs.axes
        }
        RowLayout {
            width: parent.width
            RoundButton{
                //text: "<-"
                icon.source: "qrc:/images/arrow-back.png"
                onClicked: {
                    axes_slider.decrease();
                    cvs.axes = axes_slider.value;
                }
            }
            Slider{
                id: axes_slider
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignHCenter
                from: 1
                value: 3
                to: 100
                stepSize: 1
                onMoved: {
                    cvs.axes = this.value;
                }
            }
            RoundButton{
                //text: "->"
                icon.source: "qrc:/images/arrow-forward.png"
                onClicked: {
                    axes_slider.increase();
                    cvs.axes = axes_slider.value;
                }
            }
        }
    }

    background: Rectangle {
        color:"transparent"
        Rectangle {
            width: 1
            anchors.fill: parent
            radius: 10
            color: "#414141"
        }

    }

}