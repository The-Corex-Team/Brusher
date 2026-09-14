#include <QApplication>
#include <QSurfaceFormat>
#include <QFile>
#include <QIcon>
#include <QImage>
#include <QImageReader>
#include <QDebug>

#include "ui/MainWindow.h"

int main(int argc, char *argv[])
{
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

    qDebug() << "========================================";
    qDebug() << "Brusher Resource Diagnostic";
    qDebug() << "========================================";

    const QStringList iconPaths = {
        ":/icons/brush.png",
        ":/icons/eraser.png",
        ":/icons/fill.png",
        ":/icons/move.png",
        ":/icons/pan.png",
        ":/icons/zoom.png",
        ":/icons/colorpicker.png",
        ":/icons/line.png",
        ":/icons/rectselect.png",
        ":/icons/ellipseselect.png",
        ":/icons/lassoselect.png",
        ":/icons/text.png",
        ":/icons/swap.png",
        ":/icons/layer_visible.png",
        ":/icons/layer_new.png",
        ":/icons/layer_delete.png",
        ":/icons/layer_duplicate.png"
    };

    for (const QString &path : iconPaths) {
        QFile file(path);

        qDebug() << "";
        qDebug() << "Resource:" << path;
        qDebug() << "  QFile exists:" << file.exists();

        QImage image(path);

        qDebug() << "  QImage isNull:" << image.isNull();
        qDebug() << "  QImage size:" << image.size();
        qDebug() << "  QImage format:" << image.format();

        QIcon icon(path);

        qDebug() << "  QIcon isNull:" << icon.isNull();
        qDebug() << "  QIcon sizes:" << icon.availableSizes();
    }

    qDebug() << "";
    qDebug() << "Supported image formats:";

    const auto formats = QImageReader::supportedImageFormats();

    for (const QByteArray &format : formats) {
        qDebug() << " " << format;
    }

    qDebug() << "";
    qDebug() << "========================================";

    // Application icon
    const QString iconPath = QStringLiteral(":/icons/Brusher.png");

    QIcon appIcon(iconPath);

    if (!appIcon.isNull()) {
        app.setWindowIcon(appIcon);
    }

    // Dark theme
    const QString stylePath =
        QStringLiteral(":/styles/brusher_dark.qss");

    QFile styleFile(stylePath);

    if (styleFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        app.setStyleSheet(
            QString::fromUtf8(styleFile.readAll())
        );
        styleFile.close();
    }
    else {
        qWarning()
            << "Could not open style sheet:"
            << stylePath;
    }

    MainWindow window;
    window.show();

    return app.exec();
}