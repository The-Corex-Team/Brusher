#include <QApplication>
#include <QSurfaceFormat>
#include <QFile>
#include <QIcon>
#include <QDebug>

#include "ui/MainWindow.h"

int main(int argc, char *argv[])
{
    // Let Qt choose the correct native platform plugin.
    //
    // Windows -> QWindows
    // Linux   -> Wayland/X11 according to the environment
    // macOS   -> Cocoa

    QSurfaceFormat surfaceFormat;
    surfaceFormat.setDepthBufferSize(0);
    surfaceFormat.setStencilBufferSize(0);
    surfaceFormat.setSamples(0);
    surfaceFormat.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
    QSurfaceFormat::setDefaultFormat(surfaceFormat);

    QApplication app(argc, argv);

    app.setApplicationName("Brusher");
    app.setApplicationDisplayName("Brusher");
    app.setOrganizationName("Corex Team");

    // -----------------------------------------------------------------------
    // Verify Qt resources
    // -----------------------------------------------------------------------

    const QString iconPath = QStringLiteral(":/src/icons/Brusher.svg");
    const QString stylePath = QStringLiteral(":/src/styles/brusher_dark.qss");

    QFile iconFile(iconPath);
    QFile styleFile(stylePath);

    qDebug() << "Brusher resource check:";
    qDebug() << "  Application icon:" << iconFile.exists();
    qDebug() << "  Style sheet:" << styleFile.exists();

    // -----------------------------------------------------------------------
    // Application icon
    // -----------------------------------------------------------------------

    if (iconFile.exists()) {
        QIcon appIcon(iconPath);

        qDebug() << "  Application icon isNull:" << appIcon.isNull();

        if (!appIcon.isNull()) {
            app.setWindowIcon(appIcon);
        }
    }

    // -----------------------------------------------------------------------
    // Dark theme
    // -----------------------------------------------------------------------

    if (styleFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        app.setStyleSheet(QString::fromUtf8(styleFile.readAll()));
        styleFile.close();
    } else {
        qWarning() << "Could not open style sheet:" << stylePath;
    }

    // -----------------------------------------------------------------------
    // Main window
    // -----------------------------------------------------------------------

    MainWindow window;
    window.show();

    return app.exec();
}