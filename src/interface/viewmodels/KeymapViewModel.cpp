#include "KeymapViewModel.hpp"

KeymapViewModel::KeymapViewModel(QObject* parent)
    : QObject(parent) {}

void KeymapViewModel::loadDemo() {
    m_layers = {"Base", "Lower", "Raise", "Adjust"};
    emit layersChanged();
}

void KeymapViewModel::copyText(const QString& text) {
    if (!m_clipboardPort) return;
    CopyTextToClipboard uc(*m_clipboardPort);
    uc.execute(text.toStdString());
}
