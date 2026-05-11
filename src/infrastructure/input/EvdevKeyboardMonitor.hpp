#pragma once

#include <QObject>
#include <QSocketNotifier>
#include <QString>
#include <QStringList>
#include <QThread>
#include <atomic>
#include <memory>
#include <vector>

struct libevdev;

class EvdevKeyboardMonitor : public QObject {
    Q_OBJECT
   public:
    explicit EvdevKeyboardMonitor(QObject* parent = nullptr);
    ~EvdevKeyboardMonitor() override;

    Q_INVOKABLE void start();
    Q_INVOKABLE void stop();

   signals:
    void keyEventCaptured(const QString& deviceName, int code, int value, quint64 usec);
    void warningRaised(const QString& message);

   private:
    void onThreadStarted();
    void openDevices();
    void closeDevices();

    struct DeviceHandle {
        int fd{-1};
        libevdev* dev{nullptr};
        QString name;
        QSocketNotifier* notifier{nullptr};
    };

    std::vector<DeviceHandle> m_devices;
    std::unique_ptr<QThread> m_thread;
    std::atomic<bool> m_running{false};
};
