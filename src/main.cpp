#include "ui/MainWindow.h"
#include "ui/BrusherTheme.h"

#include <QApplication>
#include <QWidget>
#include <QPixmap>
#include <QPainter>
#include <QEventLoop>
#include <QTimer>
#include <QScreen>
#include <QGuiApplication>
#include <cstdio>
#include <QIcon>
#include <QPixmapCache>
#include <QSurfaceFormat>

// Splash configuration
const double SPLASH_SCALE = 0.50;      // 50% size
const int SPLASH_DURATION_MS = 2000;  // 2 seconds

class SplashWindow : public QWidget {
public:
    SplashWindow(const QString& imagePath,
                 double scale,
                 QWidget* parent = nullptr)
        : QWidget(parent),
          m_pixmap(imagePath)
    {
        if (m_pixmap.isNull()) {
            fprintf(stderr,
                    "ERROR: Failed to load splash image: %s\n",
                    imagePath.toUtf8().constData());
            return;
        }

        QSize scaledSize(
            static_cast<int>(m_pixmap.width() * scale),
            static_cast<int>(m_pixmap.height() * scale)
        );

        m_pixmap = m_pixmap.scaled(
            scaledSize,
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation
        );

        setFixedSize(scaledSize);

        setWindowFlags(
            Qt::FramelessWindowHint |
            Qt::WindowStaysOnTopHint |
            Qt::Dialog
        );
    }

protected:
    void paintEvent(QPaintEvent*) override
    {
        QPainter painter(this);
        painter.drawPixmap(rect(), m_pixmap);
    }

private:
    QPixmap m_pixmap;
};

int main(int argc, char *argv[])
{
    QSurfaceFormat surfaceFormat;
    surfaceFormat.setDepthBufferSize(0);
    surfaceFormat.setStencilBufferSize(0);
    surfaceFormat.setSamples(0);
    surfaceFormat.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
    QSurfaceFormat::setDefaultFormat(surfaceFormat);

    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(QStringLiteral(":/src/icons/Brusher.svg")));
    QPixmapCache::setCacheLimit(1024);
    fprintf(stderr, "DEBUG: App created\n");

    {
        SplashWindow splash(
            QStringLiteral(":/src/icons/Brusher_Splashscreen.png"),
            SPLASH_SCALE
        );

        fprintf(stderr, "DEBUG: SplashWindow created\n");

        // Center splash
        if (QScreen* screen = QGuiApplication::primaryScreen()) {
            QRect g = screen->availableGeometry();

            QPoint center(
                g.center().x() - splash.width() / 2,
                g.center().y() - splash.height() / 2
            );

            splash.move(center);
        }

        splash.show();
        splash.raise();
        splash.activateWindow();

        a.processEvents();

        fprintf(stderr, "DEBUG: Splash shown\n");

        // Wait
        QEventLoop loop;
        QTimer::singleShot(
            SPLASH_DURATION_MS,
            &loop,
            &QEventLoop::quit
        );

        loop.exec();

        splash.close();
    }

    fprintf(stderr, "DEBUG: Splash closed\n");

    // Apply theme
    BrusherTheme::apply(a);

    MainWindow w;
    w.show();

    fprintf(stderr, "DEBUG: MainWindow shown\n");

    return a.exec();
}
