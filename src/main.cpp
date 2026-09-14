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
#include <QIcon>
#include <QPixmapCache>
#include <QSurfaceFormat>
#include <QDebug>

// Splash configuration
const double SPLASH_SCALE = 0.50;      // 50% size
const int SPLASH_DURATION_MS = 5000;   // 5 seconds

class SplashWindow : public QWidget {
public:
    SplashWindow(const QString& imagePath,
                 double scale,
                 QWidget* parent = nullptr)
        : QWidget(parent),
          m_pixmap(imagePath)
    {
        if (m_pixmap.isNull()) {
            qWarning() << "Failed to load splash image:" << imagePath;
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

    QApplication app(argc, argv);

    app.setApplicationName("Brusher");
    app.setApplicationDisplayName("Brusher");
    app.setOrganizationName("Corex Team");

    app.setWindowIcon(QIcon(QStringLiteral(":/icons/Brusher.png")));
    QPixmapCache::setCacheLimit(1024);

    {
        SplashWindow splash(
            QStringLiteral(":/icons/Brusher_Splashscreen.png"),
            SPLASH_SCALE
        );

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

        app.processEvents();

        QEventLoop loop;
        QTimer::singleShot(
            SPLASH_DURATION_MS,
            &loop,
            &QEventLoop::quit
        );

        loop.exec();

        splash.close();
    }

    BrusherTheme::apply(app);

    MainWindow window;
    window.show();

    return app.exec();
}