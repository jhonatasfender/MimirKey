import QtQuick 2.15
import QtQuick.Controls 2.15

Item {
    id: root
    property url source
    property rect clipRect: Qt.rect(0, 0, 100, 100)

    signal statusChanged(string status, real paintedW, real paintedH)

    Rectangle {
        anchors.fill: parent
        color: "transparent"
    }

    Image {
        id: img
        anchors.fill: parent
        fillMode: Image.PreserveAspectFit
        smooth: true
        mipmap: true
        cache: true
        source: root.source
        sourceClipRect: root.clipRect
        onStatusChanged: {
            const statuses = ["Null", "Ready", "Loading", "Error"];
            root.statusChanged(statuses[status] || String(status), paintedWidth, paintedHeight);
        }
    }
}
