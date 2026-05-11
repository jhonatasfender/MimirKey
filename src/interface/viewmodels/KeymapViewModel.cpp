#include "KeymapViewModel.hpp"

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QProcess>

#if KEYMAPS_ENABLE_BLE
#include <QBluetoothUuid>

#include "src/infrastructure/ble/ZmkBleLayerMonitor.hpp"
#endif

KeymapViewModel::KeymapViewModel(QObject* parent)
    : QObject(parent) {
}

void KeymapViewModel::loadDemo() {
    m_layers = {"Base", "Lower", "Raise", "Adjust"};
    emit layersChanged();
}

void KeymapViewModel::copyText(const QString& text) {
    if (!m_clipboardPort) return;
    CopyTextToClipboard uc(*m_clipboardPort);
    uc.execute(text.toStdString());
}

void KeymapViewModel::log(const QString& level, const QString& message, const QString& metaJson) {
    emit logMessage(level, message, metaJson);
}

void KeymapViewModel::requestEvdevPermissions() {
    QString scriptPath = QCoreApplication::applicationDirPath() + "/grant_evdev_access.sh";
    if (!QFileInfo::exists(scriptPath)) {
        scriptPath = QDir(QCoreApplication::applicationDirPath()).absoluteFilePath("../scripts/grant_evdev_access.sh");
    }
    QString user = qEnvironmentVariable("USER");
    if (user.isEmpty()) user = qEnvironmentVariable("SUDO_USER");
    QStringList pkArgs;
    pkArgs << "/bin/bash" << scriptPath;
    if (!user.isEmpty()) pkArgs << user;
    QProcess::startDetached("pkexec", pkArgs);
}

#if KEYMAPS_ENABLE_BLE
void KeymapViewModel::setBleMonitor(ZmkBleLayerMonitor* monitor) {
    m_bleMonitor = monitor;
    if (!m_bleMonitor) return;
    QObject::connect(m_bleMonitor, &ZmkBleLayerMonitor::layerChanged, this, [this](int layer) {
        setActiveLayer(layer);
        emit logMessage("info", "layer.changed", QString::number(layer));
    });
}

void KeymapViewModel::setBleTarget(const QString& macAddress,
                                   const QString& serviceUuid,
                                   const QString& characteristicUuid) {
    m_bleMac = macAddress;
    m_bleServiceUuid = serviceUuid;
    m_bleCharUuid = characteristicUuid;
}

void KeymapViewModel::startBle() {
    if (!m_bleMonitor) {
        emit logMessage("warn", "ble.error", QStringLiteral("No monitor"));
        return;
    }
    if (m_bleMac.isEmpty() || m_bleServiceUuid.isEmpty() || m_bleCharUuid.isEmpty()) {
        emit logMessage("warn", "ble.error", QStringLiteral("Missing MAC/UUIDs"));
        return;
    }
    m_bleMonitor->setDeviceAddress(m_bleMac);
    m_bleMonitor->setServiceUuid(QBluetoothUuid(m_bleServiceUuid));
    m_bleMonitor->setCharacteristicUuid(QBluetoothUuid(m_bleCharUuid));
    m_bleMonitor->start();
}

void KeymapViewModel::startBleAuto() {
    if (!m_bleMonitor) {
        emit logMessage("warn", "ble.error", QStringLiteral("No monitor"));
        return;
    }

    m_bleMonitor->stop();
    m_bleMonitor->startAuto();
}
#endif