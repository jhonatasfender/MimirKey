#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "infrastructure/fs/QtClipboardRepository.hpp"
#include "infrastructure/input/EvdevKeyboardMonitor.hpp"
#include "interface/viewmodels/KeymapViewModel.hpp"
#if KEYMAPS_ENABLE_BLE
#include "infrastructure/ble/ZmkBleLayerMonitor.hpp"
#endif

int main(int argc, char* argv[]) {
    QGuiApplication app(argc, argv);

    qmlRegisterType<KeymapViewModel>("Keymaps", 1, 0, "KeymapViewModel");

    QQmlApplicationEngine engine;

    QtClipboardRepository clipboardRepo;
    KeymapViewModel vm;
    vm.setClipboardPort(&clipboardRepo);

    auto* evdev = new EvdevKeyboardMonitor(nullptr);
    QObject::connect(evdev, &EvdevKeyboardMonitor::keyEventCaptured, &vm,
                     &KeymapViewModel::keyEventCaptured);
    evdev->start();
    QObject::connect(&app, &QCoreApplication::aboutToQuit, evdev, &EvdevKeyboardMonitor::stop);
    QObject::connect(&app, &QCoreApplication::aboutToQuit, evdev, &QObject::deleteLater);

#if KEYMAPS_ENABLE_BLE
    auto* ble = new ZmkBleLayerMonitor(&vm);
    QObject::connect(ble, &ZmkBleLayerMonitor::layerChanged, &vm, [&](int layer) {
        vm.setActiveLayer(layer);
    });
#endif
    engine.rootContext()->setContextProperty("vmInjected", &vm);
    engine.loadFromModule(u"Keymaps"_qs, u"Main"_qs);

    return app.exec();
}