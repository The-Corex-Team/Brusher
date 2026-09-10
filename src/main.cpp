#include <QApplication>
#include <QSurfaceFormat>
#include <QFile>
#include <QIcon>
#include <QPixmap>
#include <QPalette>
#include <QColor>
#include <cstdio>

#include "ui/MainWindow.h"

int main(int argc, char *argv[])
{
    // Let Qt automatically select the native platform plugin:
    // Windows -> qwindows
    // Linux   -> Wayland/X11 according to the environment
    // macOS   -> Cocoa

    QSurfaceFormat surfaceFormat;
    surfaceFormat.setDepthBufferSize(0);
    surfaceFormat.setStencilBufferSize(0);
    surfaceFormat.setSamples(0);
    surfaceFormat.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
    QSurfaceFormat::setDefaultFormat(surfaceFormat);

    QApplication a(argc, argv);

    fprintf(stderr, "DEBUG: QApplication created\n");

    a.setApplicationName("Brusher");
    a.setApplicationDisplayName("Brusher");
    a.setOrganizationName("Corex Team");

    QIcon appIcon(":/src/icons/Brusher.svg");
    a.setWindowIcon(appIcon);

    QFile styleFile(":/src/styles/brusher_dark.qss");
    if (styleFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        a.setStyleSheet(QString::fromUtf8(styleFile.readAll()));
        styleFile.close();
    }

    MainWindow w;
    w.show();

    return a.exec();
}
