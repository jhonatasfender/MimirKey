#pragma once

#include <QObject>
#include <QString>
#include <QStringList>

#include "src/application/usecases/CopyText.hpp"

class KeymapViewModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(QStringList layers READ layers NOTIFY layersChanged)
   public:
    explicit KeymapViewModel(QObject* parent = nullptr);

    void setClipboardPort(ClipboardPort* port) { m_clipboardPort = port; }
    Q_INVOKABLE void copyText(const QString& text);

    QStringList layers() const { return m_layers; }

    Q_INVOKABLE void loadDemo();

   signals:
    void layersChanged();

   private:
    QStringList m_layers;
    ClipboardPort* m_clipboardPort{nullptr};
};
