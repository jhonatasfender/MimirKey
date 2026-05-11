#pragma once

#include <QObject>
#include <QString>
#include <QStringList>

#include "src/application/usecases/CopyText.hpp"

class KeymapViewModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(QStringList layers READ layers NOTIFY layersChanged)
    Q_PROPERTY(int activeLayer READ activeLayer NOTIFY activeLayerChanged)

   public:
    explicit KeymapViewModel(QObject* parent = nullptr);

    void setClipboardPort(ClipboardPort* port) { m_clipboardPort = port; }
    Q_INVOKABLE void copyText(const QString& text);
    Q_INVOKABLE void log(const QString& level, const QString& message, const QString& metaJson);
    Q_INVOKABLE void requestEvdevPermissions();

#if KEYMAPS_ENABLE_BLE
    Q_INVOKABLE void setBleTarget(const QString& macAddress,
                                  const QString& serviceUuid,
                                  const QString& characteristicUuid);
    Q_INVOKABLE void startBle();
    Q_INVOKABLE void startBleAuto();
#endif

    QStringList layers() const { return m_layers; }
    int activeLayer() const { return m_activeLayer; }

    void setActiveLayer(int layer) {
        if (m_activeLayer == layer) return;
        m_activeLayer = layer;
        emit activeLayerChanged();
    }

    Q_INVOKABLE void loadDemo();

   signals:
    void layersChanged();
    void activeLayerChanged();
    void keyEventCaptured(const QString& deviceName, int code, int value, quint64 usec);
    void logMessage(const QString& level, const QString& message, const QString& metaJson);

   private:
    QStringList m_layers;
    ClipboardPort* m_clipboardPort{nullptr};
    int m_activeLayer{0};

#if KEYMAPS_ENABLE_BLE
   public:
    void setBleMonitor(class ZmkBleLayerMonitor* monitor);

   private:
    class ZmkBleLayerMonitor* m_bleMonitor{nullptr};
    QString m_bleMac;
    QString m_bleServiceUuid;
    QString m_bleCharUuid;
#endif
};