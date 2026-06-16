#include "BrusherTheme.h"

#include <QApplication>
#include <QFile>
#include <QPalette>
#include <QStyleFactory>

void BrusherTheme::apply(QApplication &app)
{
    // 1. Force Fusion style — the cross-platform rendering backend.
    //    This eliminates all native widget rendering (Windows Aero,
    //    GNOME Adwaita, macOS Aqua) and provides a consistent base.
    app.setStyle(QStyleFactory::create("Fusion"));

    // 2. Set a comprehensive dark QPalette.
    //    This catches widgets/sub-widgets that QSS doesn't fully reach
    //    (e.g., QColorDialog color wheel, native file dialog fallbacks).
    QPalette darkPalette;

    // Window & base
    darkPalette.setColor(QPalette::Window,          QColor(0x53, 0x53, 0x53));
    darkPalette.setColor(QPalette::WindowText,      QColor(0xcc, 0xcc, 0xcc));
    darkPalette.setColor(QPalette::Base,            QColor(0x1e, 0x1e, 0x1e));
    darkPalette.setColor(QPalette::AlternateBase,   QColor(0x28, 0x28, 0x28));

    // Text
    darkPalette.setColor(QPalette::Text,            QColor(0xcc, 0xcc, 0xcc));
    darkPalette.setColor(QPalette::BrightText,      QColor(0xff, 0xff, 0xff));
    darkPalette.setColor(QPalette::PlaceholderText, QColor(0x8a, 0x8a, 0x8a));

    // Buttons
    darkPalette.setColor(QPalette::Button,          QColor(0x3c, 0x3c, 0x3c));
    darkPalette.setColor(QPalette::ButtonText,      QColor(0xcc, 0xcc, 0xcc));

    // Tooltips
    darkPalette.setColor(QPalette::ToolTipBase,     QColor(0x1e, 0x1e, 0x1e));
    darkPalette.setColor(QPalette::ToolTipText,     QColor(0xcc, 0xcc, 0xcc));

    // Highlights (selection, focus) - PS Blue
    darkPalette.setColor(QPalette::Highlight,       QColor(0x2d, 0x8c, 0xeb));
    darkPalette.setColor(QPalette::HighlightedText, QColor(0xff, 0xff, 0xff));

    // Links
    darkPalette.setColor(QPalette::Link,            QColor(0x2d, 0x8c, 0xeb));
    darkPalette.setColor(QPalette::LinkVisited,     QColor(0x1a, 0x6b, 0xc4));

    // Structural
    darkPalette.setColor(QPalette::Dark,            QColor(0x19, 0x19, 0x19));
    darkPalette.setColor(QPalette::Shadow,          QColor(0x15, 0x15, 0x15));
    darkPalette.setColor(QPalette::Mid,             QColor(0x28, 0x28, 0x28));
    darkPalette.setColor(QPalette::Midlight,        QColor(0x3c, 0x3c, 0x3c));
    darkPalette.setColor(QPalette::Light,           QColor(0x4a, 0x4a, 0x4a));

    // Disabled state overrides
    darkPalette.setColor(QPalette::Disabled, QPalette::WindowText,  QColor(0x99, 0x99, 0x99));
    darkPalette.setColor(QPalette::Disabled, QPalette::Text,        QColor(0x99, 0x99, 0x99));
    darkPalette.setColor(QPalette::Disabled, QPalette::ButtonText,  QColor(0x99, 0x99, 0x99));
    darkPalette.setColor(QPalette::Disabled, QPalette::Highlight,   QColor(0x33, 0x33, 0x33));

    app.setPalette(darkPalette);

    // 3. Load the comprehensive QSS stylesheet from resources.
    QFile styleFile(":/src/styles/brusher_dark.qss");
    if (styleFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        app.setStyleSheet(styleFile.readAll());
        styleFile.close();
    }
}
