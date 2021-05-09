import QtQuick 2.5
import QtQuick.Controls 2.5
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.4

ApplicationWindow {
    visible: true
    width: 640
    height: 480
    title: qsTr("Ttile 1")

    signal button1_signal(string msg)

    property alias button1: button1

    RowLayout {
        spacing: 10
        anchors.fill: parent
//        Rectangle {
//            Layout.alignment: Qt.AlignHCenter
//            color: "#000000"
//            anchors.fill: parent
//
//        }
        Label {
            id: label1
            text: qsTr("Label 1")
            color: "#FF1800"
            Layout.alignment: Qt.AlignHCenter
        }
        Label {
            id: label2
            text: qsTr("Label 2")
            color: "#FF1800"
            Layout.alignment: Qt.AlignHCenter
        }
    }

    ColumnLayout {
        spacing: 10
        anchors.fill: parent
        Button {
            id: button1
            Layout.alignment: Qt.AlignCenter
            text: qsTr("This is a button")
            onClicked: button1_signal("Message from qml")
        }
    }
}
