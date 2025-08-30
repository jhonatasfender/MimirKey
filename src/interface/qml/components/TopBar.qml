import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15

Item {
    id: root
    property alias title: titleLabel.text
    property var layersModel: []

    signal layerSelected(int index, string name)

    // Optional BLE controls
    property string bleMac: ""
    property string bleServiceUuid: ""
    property string bleCharUuid: ""

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
        TextField {
            id: macField
            placeholderText: "BLE MAC"
            text: root.bleMac
            width: 180
        }
        TextField {
            id: svcField
            placeholderText: "Service UUID"
            text: root.bleServiceUuid
            width: 230
        }
        TextField {
            id: chrField
            placeholderText: "Char UUID"
            text: root.bleCharUuid
            width: 230
        }
        Button {
            text: "Connect BLE"
            onClicked: {
                if (typeof vmInjected.setBleTarget === 'function' && typeof vmInjected.startBle === 'function') {
                    vmInjected.setBleTarget(macField.text, svcField.text, chrField.text);
                    vmInjected.startBle();
                }
            }
        }
        Button {
            text: "Light"
        }
    }
}
