import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15 as QQC
import Keymaps 1.0

ApplicationWindow {
    id: root
    visible: true
    width: 1200
    height: 800
    title: qsTr("Sofle Keymap Mapper")

    color: "#0a0a0a"

    header: ToolBar {
        contentItem: TopBar {
            anchors.fill: parent
            title: root.title
            layersModel: vmInjected.layers
            onLayerSelected: function (index, name) {
                vmInjected.log("info", "layer.selected", JSON.stringify({
                            index: index,
                            layer: name
                        }));
            }
        }
    }

    MainContent {
        id: mainContent
        anchors.fill: parent
    }

    Component.onCompleted: vmInjected.loadDemo()
}
