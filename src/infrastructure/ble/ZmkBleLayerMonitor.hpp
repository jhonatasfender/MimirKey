#pragma once

#include <QBluetoothUuid>
#include <QObject>
#include <memory>

class QLowEnergyController;
class QLowEnergyService;
class QLowEnergyCharacteristic;

class ZmkBleLayerMonitor : public QObject {
    Q_OBJECT
   public:
    explicit ZmkBleLayerMonitor(QObject* parent = nullptr);
    ~ZmkBleLayerMonitor() override;

    void setDeviceAddress(const QString& address);
    void setServiceUuid(const QBluetoothUuid& service);
    void setCharacteristicUuid(const QBluetoothUuid& ch);

    Q_INVOKABLE void start();
    Q_INVOKABLE void stop();

   signals:
    void layerChanged(int activeLayer);
    void warningRaised(const QString& message);

   private slots:
    void onConnected();
    void onDisconnected();
    void onServiceDiscovered(const QBluetoothUuid&);
    void onDiscoveryFinished();
    void onServiceStateChanged(QLowEnergyService::ServiceState);
    void onCharacteristicChanged(const QLowEnergyCharacteristic&, const QByteArray&);

   private:
    QString m_address;
    QBluetoothUuid m_serviceUuid;
    QBluetoothUuid m_charUuid;

    QLowEnergyController* m_ctrl{nullptr};
    QLowEnergyService* m_service{nullptr};
};
