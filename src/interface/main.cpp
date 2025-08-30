#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "infrastructure/fs/QtClipboardRepository.hpp"
#include "interface/viewmodels/KeymapViewModel.hpp"

int main(int argc, char* argv[]) {
    QGuiApplication app(argc, argv);

    qmlRegisterType<KeymapViewModel>("Keymaps", 1, 0, "KeymapViewModel");

    QQmlApplicationEngine engine;

    QtClipboardRepository clipboardRepo;
    KeymapViewModel vm;
    vm.setClipboardPort(&clipboardRepo);
    engine.rootContext()->setContextProperty("vmInjected", &vm);
    engine.loadFromModule(u"Keymaps"_qs, u"Main"_qs);

    return app.exec();
}
