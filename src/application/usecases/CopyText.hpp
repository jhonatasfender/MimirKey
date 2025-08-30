#pragma once

#include <string>

class ClipboardPort {
   public:
    virtual ~ClipboardPort() = default;
    virtual void setText(const std::string& text) = 0;
};

class CopyTextToClipboard {
   public:
    explicit CopyTextToClipboard(ClipboardPort& clipboard) : m_clipboard(clipboard) {}
    void execute(const std::string& text) { m_clipboard.setText(text); }

   private:
    ClipboardPort& m_clipboard;
};
