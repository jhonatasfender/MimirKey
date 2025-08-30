#include "ZmkBleLayerMonitor.hpp"

#include <QBluetoothAddress>
#include <QLowEnergyCharacteristic>
#include <QLowEnergyController>
#include <QLowEnergyDescriptor>
#include <QLowEnergyService>

ZmkBleLayerMonitor::ZmkBleLayerMonitor(QObject* parent) : QObject(parent) {
}

ZmkBleLayerMonitor::~ZmkBleLayerMonitor() { stop(); }

void ZmkBleLayerMonitor::setDeviceAddress(const QString& address) { m_address = address; }
void ZmkBleLayerMonitor::setServiceUuid(const QBluetoothUuid& service) { m_serviceUuid = service; }
void ZmkBleLayerMonitor::setCharacteristicUuid(const QBluetoothUuid& ch) { m_charUuid = ch; }

void ZmkBleLayerMonitor::start() {
    if (m_ctrl) return;
    if (m_address.isEmpty() || m_serviceUuid.isNull() || m_charUuid.isNull()) {
        emit warningRaised("BLE not configured: address/service/characteristic missing");
        return;
    }
    m_ctrl = QLowEnergyController::createCentral(QBluetoothAddress(m_address));
    emit warningRaised(QStringLiteral("BLE connecting to ") + m_address);
    connect(m_ctrl, &QLowEnergyController::connected, this, &ZmkBleLayerMonitor::onConnected);
    connect(m_ctrl, &QLowEnergyController::disconnected, this, &ZmkBleLayerMonitor::onDisconnected);
    connect(m_ctrl, &QLowEnergyController::serviceDiscovered, this, &ZmkBleLayerMonitor::onServiceDiscovered);
    connect(m_ctrl, &QLowEnergyController::discoveryFinished, this, &ZmkBleLayerMonitor::onDiscoveryFinished);
    connect(m_ctrl, &QLowEnergyController::errorOccurred, this, [this](QLowEnergyController::Error e) {
        emit warningRaised(QStringLiteral("BLE error: ") + QString::number(int(e)));
    });
    connect(m_ctrl, &QLowEnergyController::stateChanged, this, [this](QLowEnergyController::ControllerState s) {
        emit warningRaised(QStringLiteral("BLE state: ") + QString::number(int(s)));
    });
    m_ctrl->connectToDevice();
}

void ZmkBleLayerMonitor::stop() {
    if (m_service) {
        m_service->deleteLater();
        m_service = nullptr;
    }
    if (m_ctrl) {
        m_ctrl->disconnectFromDevice();
        m_ctrl->deleteLater();
        m_ctrl = nullptr;
    }
}

void ZmkBleLayerMonitor::onConnected() {
    emit warningRaised("BLE connected, discovering services...");
    m_ctrl->discoverServices();
}

void ZmkBleLayerMonitor::onDisconnected() {
    emit warningRaised("BLE disconnected");
}

void ZmkBleLayerMonitor::onServiceDiscovered(const QBluetoothUuid& uuid) {
    if (uuid == m_serviceUuid) {
        if (m_service) return;
        m_service = m_ctrl->createServiceObject(uuid);
        if (!m_service) {
            emit warningRaised("Failed to create service object for target UUID");
            return;
        }
        connect(m_service, &QLowEnergyService::stateChanged, this, &ZmkBleLayerMonitor::onServiceStateChanged);
        connect(m_service, &QLowEnergyService::characteristicChanged, this, &ZmkBleLayerMonitor::onCharacteristicChanged);
        connect(m_service, &QLowEnergyService::characteristicRead, this, &ZmkBleLayerMonitor::onCharacteristicRead);
        m_service->discoverDetails();
        emit warningRaised("Target service discovered; discovering details...");
    }
}

void ZmkBleLayerMonitor::onDiscoveryFinished() {
    if (!m_service) emit warningRaised("ZMK Studio service not found");
}

void ZmkBleLayerMonitor::onServiceStateChanged(QLowEnergyService::ServiceState s) {
    if (s == QLowEnergyService::ServiceDiscovered) {
        auto ch = m_service->characteristic(m_charUuid);
        if (!ch.isValid()) {
            emit warningRaised("ZMK layer characteristic not found");
            return;
        }
        auto cccd = ch.descriptor(QBluetoothUuid::DescriptorType::ClientCharacteristicConfiguration);
        if (cccd.isValid()) {
            m_service->writeDescriptor(cccd, QByteArray::fromHex("0100"));
            emit warningRaised("Subscribed to notifications (CCCD 0x0100)");
        }

        m_service->readCharacteristic(ch);
        emit warningRaised("Requested initial read of layer characteristic");
    }
}

void ZmkBleLayerMonitor::onCharacteristicChanged(const QLowEnergyCharacteristic& ch, const QByteArray& value) {
    Q_UNUSED(ch);
    if (value.isEmpty()) return;
    const int layer = static_cast<unsigned char>(value[0]);
    emit warningRaised(QStringLiteral("Layer value rx: ") + QString::number(layer));
    emit layerChanged(layer);
}

void ZmkBleLayerMonitor::onCharacteristicRead(const QLowEnergyCharacteristic& ch, const QByteArray& value) {
    if (ch.uuid() != m_charUuid) return;
    if (value.isEmpty()) return;
    const int layer = static_cast<unsigned char>(value[0]);
    emit warningRaised(QStringLiteral("Layer value read: ") + QString::number(layer));
    emit layerChanged(layer);
}