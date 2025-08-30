import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: root

    property rect svgViewRect: Qt.rect(0, 0, 956, 390)

    ListModel {
        id: logsModel
    }

    function log(level, message, meta) {
        const entry = {
            ts: new Date().toISOString(),
            level: level,
            message: message,
            meta: meta ? JSON.stringify(meta) : ""
        };
        logsModel.append(entry);
        if (logsModel.count > 500)
            logsModel.remove(0, logsModel.count - 500);
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 16

        Frame {
            Layout.fillWidth: true
            Layout.fillHeight: true
            padding: 0
            background: Rectangle {
                color: "#111318"
                radius: 8
                border.color: "#1f2937"
            }

            KeymapView {
                id: keymapView
                anchors.fill: parent
                anchors.margins: 16
                source: Qt.resolvedUrl("../assets/keymap.svg")
                clipRect: root.svgViewRect
                onStatusChanged: function (status, w, h) {
                    root.log("debug", "svg.status", {
                            status: status,
                            paintedWidth: w,
                            paintedHeight: h,
                            clip: {
                                x: root.svgViewRect.x,
                                y: root.svgViewRect.y,
                                w: root.svgViewRect.width,
                                h: root.svgViewRect.height
                            }
                        });
                }
            }
        }

        Frame {
            Layout.fillWidth: true
            Layout.preferredHeight: 180
            padding: 0
            background: Rectangle {
                color: "#0f1115"
                radius: 8
                border.color: "#1f2937"
            }

            LogPanel {
                id: logPanel
                anchors.fill: parent
                model: logsModel
                onClearRequested: logsModel.clear()
                onCopyRequested: {
                    let out = "";
                    for (let i = 0; i < logsModel.count; ++i) {
                        const it = logsModel.get(i);
                        out += JSON.stringify(it) + "\n";
                    }
                    vmInjected.copyText(out);
                }
            }
        }
    }

    Connections {
        target: vmInjected

        function onKeyEventCaptured(deviceName, code, value, usec) {
            root.log(value ? "info" : "debug", "key.event", {
                    device: deviceName,
                    code: code,
                    value: value,
                    usec: usec
                });
        }

        function onLogMessage(level, message, metaJson) {
            let metaObj = undefined;
            try {
                metaObj = metaJson ? JSON.parse(metaJson) : undefined;
            } catch (e) {
                metaObj = {
                    parseError: String(e),
                    raw: metaJson
                };
            }
            root.log(level, message, metaObj);
        }

        function onLayersChanged() {
            root.log("info", "layers.loaded", {
                    count: vmInjected.layers.length,
                    names: vmInjected.layers
                });
        }
    }

    Button {
        id: permBtn
        text: "Grant evdev access"
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: 12
        visible: false
        onClicked: vmInjected.requestEvdevPermissions()
    }

    Connections {
        target: vmInjected
        function onLogMessage(level, message, metaJson) {
            if (message === "evdev.warning" && metaJson && metaJson.indexOf("Permission denied") !== -1) {
                permBtn.visible = true;
            }
        }
    }
}
