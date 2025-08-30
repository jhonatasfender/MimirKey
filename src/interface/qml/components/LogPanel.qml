import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import Qt.labs.platform 1.1 as Platform

Item {
    id: root
        signal
    clearRequested
        signal
    copyRequested
    property alias model: listView.model

    Rectangle {
        anchors.fill: parent
        color: "transparent"
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 8
        spacing: 6
        RowLayout {
            Layout.fillWidth: true
            Label {
                text: "Logs"
                color: "#a3a3a3"
                font.bold: true
                Layout.fillWidth: true
            }
            Button {
                text: "Copy"
                onClicked: root.copyRequested()
            }
            Button {
                text: "Clear"
                onClicked: root.clearRequested()
            }
        }
        ListView {
            id: listView
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            delegate: Row {
                spacing: 6
                Text {
                    text: ts
                    color: "#64748b"
                }
                Text {
                    text: level
                    color: level === "error" ? "#f87171" : (level === "warn" ? "#fbbf24" : "#93c5fd")
                }
                Text {
                    text: message
                    color: "#e5e7eb"
                }
                Text {
                    text: meta
                    color: "#9ca3af"
                    elide: Text.ElideRight
                    width: listView.width * 0.4
                }
            }
            onCountChanged: positionViewAtEnd()
        }
    }
}
