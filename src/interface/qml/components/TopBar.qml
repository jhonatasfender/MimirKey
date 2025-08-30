import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15

Item {
    id: root
    property alias title: titleLabel.text
    property var layersModel: []
    signal layerSelected(int index, string name)

    implicitHeight: 48
    anchors.fill: parent

    RowLayout {
        anchors.fill: parent
        Label {
            id: titleLabel
            color: "#e5e7eb"
            font.bold: true
            Layout.fillWidth: true
        }
        Label {
            text: "Layer"
            color: "#a3a3a3"
        }
        ComboBox {
            id: layerCombo
            model: root.layersModel
            width: 160
            onCurrentIndexChanged: root.layerSelected(currentIndex, currentText)
        }
        Button {
            text: "Light"
        }
    }
}
