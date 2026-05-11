#include "ZmkBleLayerMonitor.hpp"

#include <QBluetoothAddress>
#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothDeviceInfo>
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
    if (m_address.isEmpty()) {
        emit warningRaised("BLE not configured: address missing (auto discovery not started)");
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

void ZmkBleLayerMonitor::startAuto() {
    if (m_ctrl || m_discovery) return;
    emit warningRaised("BLE scanning for devices...");
    m_discovery = new QBluetoothDeviceDiscoveryAgent(this);
    m_discovery->setLowEnergyDiscoveryTimeout(5000);
    connect(m_discovery, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered, this,
            &ZmkBleLayerMonitor::onDeviceDiscovered);
    connect(m_discovery, &QBluetoothDeviceDiscoveryAgent::finished, this,
            &ZmkBleLayerMonitor::onDeviceScanFinished);
    connect(m_discovery, &QBluetoothDeviceDiscoveryAgent::errorOccurred, this,
            [this](QBluetoothDeviceDiscoveryAgent::Error e) {
                emit warningRaised(QStringLiteral("BLE scan error: ") + QString::number(int(e)));
            });
    m_discovery->start(QBluetoothDeviceDiscoveryAgent::LowEnergyMethod);
}

void ZmkBleLayerMonitor::stop() {
    if (m_discovery) {
        m_discovery->stop();
        m_discovery->deleteLater();
        m_discovery = nullptr;
    }
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
    if (!m_serviceUuid.isNull() && uuid != m_serviceUuid) return;
    if (m_service) return;
    m_service = m_ctrl->createServiceObject(uuid);
    if (!m_service) {
        emit warningRaised("Failed to create service object");
        return;
    }
    connect(m_service, &QLowEnergyService::stateChanged, this, &ZmkBleLayerMonitor::onServiceStateChanged);
    connect(m_service, &QLowEnergyService::characteristicChanged, this, &ZmkBleLayerMonitor::onCharacteristicChanged);
    connect(m_service, &QLowEnergyService::characteristicRead, this, &ZmkBleLayerMonitor::onCharacteristicRead);
    m_service->discoverDetails();
    emit warningRaised("Service discovered; discovering details...");
}

void ZmkBleLayerMonitor::onDiscoveryFinished() {
    if (!m_service) emit warningRaised("BLE: no service selected during discovery");
}

void ZmkBleLayerMonitor::onServiceStateChanged(QLowEnergyService::ServiceState s) {
    if (s == QLowEnergyService::ServiceDiscovered) {
        QLowEnergyCharacteristic targetChar;
        if (!m_charUuid.isNull()) {
            targetChar = m_service->characteristic(m_charUuid);
        } else {
            const auto chars = m_service->characteristics();
            for (const auto& c : chars) {
                const auto props = c.properties();
                if (props.testFlag(QLowEnergyCharacteristic::Notify) ||
                    props.testFlag(QLowEnergyCharacteristic::Indicate)) {
                    targetChar = c;
                    m_charUuid = c.uuid();
                    emit warningRaised(QStringLiteral("Auto-selected characteristic: ") + m_charUuid.toString());
                    break;
                }
            }
        }
        if (!targetChar.isValid()) {
            emit warningRaised("No suitable characteristic found");
            return;
        }
        auto cccd = targetChar.descriptor(QBluetoothUuid::DescriptorType::ClientCharacteristicConfiguration);
        if (cccd.isValid()) {
            m_service->writeDescriptor(cccd, QByteArray::fromHex("0100"));
            emit warningRaised("Subscribed to notifications (CCCD 0x0100)");
        }

        m_service->readCharacteristic(targetChar);
        emit warningRaised("Requested initial read of characteristic");
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

void ZmkBleLayerMonitor::onDeviceDiscovered(const QBluetoothDeviceInfo& info) {
    const QString name = info.name().toLower();

    if (name.contains("zmk") || name.contains("keyboard") || name.contains("sofle")) {
        m_address = info.address().toString();
        emit warningRaised(QStringLiteral("Auto-selected device: ") + info.name() + " (" + m_address + ")");
        if (m_discovery) {
            m_discovery->stop();
        }
        start();
    }
}

void ZmkBleLayerMonitor::onDeviceScanFinished() {
    if (m_address.isEmpty()) {
        emit warningRaised("BLE scan finished: no matching device found");
    }
}