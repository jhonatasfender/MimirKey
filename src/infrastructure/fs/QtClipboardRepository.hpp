#pragma once

#include <QtCore/QObject>
#include <QtGui/QClipboard>
#include <QtGui/QGuiApplication>
#include <string>

#include "src/application/usecases/CopyText.hpp"

class QtClipboardRepository : public QObject, public ClipboardPort {
    Q_OBJECT
   public:
    explicit QtClipboardRepository(QObject* parent = nullptr) : QObject(parent) {}

    void setText(const std::string& text) override {
        if (QClipboard* cb = QGuiApplication::clipboard()) {
            const QString qtext = QString::fromStdString(text);
            cb->setText(qtext, QClipboard::Clipboard);
            if (cb->supportsSelection()) {
                cb->setText(qtext, QClipboard::Selection);
            }
            if (cb->supportsFindBuffer()) {
                cb->setText(qtext, QClipboard::FindBuffer);
            }
        }
    }
};
