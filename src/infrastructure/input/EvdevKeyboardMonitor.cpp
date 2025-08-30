#include "EvdevKeyboardMonitor.hpp"

#include <fcntl.h>
#include <libevdev/libevdev.h>
#include <linux/input.h>
#include <unistd.h>

#include <QFileInfo>
#include <QSocketNotifier>
#include <QTime>
#include <chrono>
#include <string>
#include <system_error>

namespace {
bool isKeyboard(libevdev* dev) {
    if (!libevdev_has_event_type(dev, EV_KEY)) return false;
    if (libevdev_has_event_code(dev, EV_KEY, KEY_A) && libevdev_has_event_code(dev, EV_KEY, KEY_Z)) return true;
    return libevdev_has_event_code(dev, EV_KEY, KEY_ENTER);
}
}  // namespace

EvdevKeyboardMonitor::EvdevKeyboardMonitor(QObject* parent)
    : QObject(parent) {}

EvdevKeyboardMonitor::~EvdevKeyboardMonitor() { stop(); }

void EvdevKeyboardMonitor::start() {
    if (m_running.exchange(true)) return;
    m_thread = std::make_unique<QThread>();
    connect(m_thread.get(), &QThread::started, this, &EvdevKeyboardMonitor::runLoop);
    this->moveToThread(m_thread.get());
    m_thread->start();
}

void EvdevKeyboardMonitor::stop() {
    if (!m_running.exchange(false)) return;
    if (m_thread) {
        m_thread->quit();
        m_thread->wait();
        m_thread.reset();
    }
    closeDevices();
}

void EvdevKeyboardMonitor::openDevices() {
    closeDevices();
    for (int i = 0; i < 64; ++i) {
        char path[64];
        snprintf(path, sizeof(path), "/dev/input/event%d", i);
        int fd = ::open(path, O_RDONLY | O_NONBLOCK);
        if (fd < 0) continue;
        libevdev* dev = nullptr;
        if (libevdev_new_from_fd(fd, &dev) < 0) {
            ::close(fd);
            continue;
        }
        if (!isKeyboard(dev)) {
            libevdev_free(dev);
            ::close(fd);
            continue;
        }
        DeviceHandle h;
        h.fd = fd;
        h.dev = dev;
        h.name = QString::fromUtf8(libevdev_get_name(dev));
        m_devices.push_back(h);
    }
    if (m_devices.empty()) emit warningRaised("No keyboard-like evdev devices found.");
}

void EvdevKeyboardMonitor::closeDevices() {
    for (auto& h : m_devices) {
        if (h.dev) libevdev_free(h.dev);
        if (h.fd >= 0) ::close(h.fd);
    }
    m_devices.clear();
}

void EvdevKeyboardMonitor::runLoop() {
    openDevices();
    while (m_running.load()) {
        for (auto& h : m_devices) {
            input_event ev;
            int rc = libevdev_next_event(h.dev, LIBEVDEV_READ_FLAG_NORMAL, &ev);
            if (rc == 0) {
                if (ev.type == EV_KEY) {
                    quint64 ts = static_cast<quint64>(ev.time.tv_sec) * 1000000ull + static_cast<quint64>(ev.time.tv_usec);
                    emit keyEventCaptured(h.name, ev.code, ev.value, ts);
                }
            }
        }
        usleep(2000);
    }
}
