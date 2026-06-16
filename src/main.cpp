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

// Splash window configuration
const int SPLASH_WIDTH = 1080;
const int SPLASH_HEIGHT = 720;
const int SPLASH_DURATION_MS = 10000; // 10 seconds

class SplashWindow : public QWidget {
public:
    SplashWindow(const QString &imagePath,
                 int width,
                 int height,
                 QWidget *parent = nullptr)
        : QWidget(parent),
          m_pixmap(imagePath),
          m_width(width),
          m_height(height)
    {
        setFixedSize(m_width, m_height);

        setWindowFlags(
            Qt::FramelessWindowHint |
            Qt::WindowStaysOnTopHint |
            Qt::Dialog
        );

        if (m_pixmap.isNull()) {
            fprintf(stderr, "ERROR: Failed to load splash image: %s\n",
                    imagePath.toUtf8().constData());
        }
    }

protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter painter(this);
        painter.setRenderHint(QPainter::SmoothPixmapTransform);

        painter.drawPixmap(rect(), m_pixmap);
    }

private:
    QPixmap m_pixmap;
    int m_width;
    int m_height;
};

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    fprintf(stderr, "DEBUG: App created\n");

    SplashWindow splash(
        QStringLiteral(":/src/icons/Brusher_Splashscreen.png"),
        SPLASH_WIDTH,
        SPLASH_HEIGHT
    );

    fprintf(stderr, "DEBUG: SplashWindow created\n");

    // Center on screen
    QScreen *screen = QGuiApplication::primaryScreen();

    if (screen) {
        QRect g = screen->availableGeometry();
        QPoint center = g.center() - splash.rect().center();
        splash.move(center);
    }

    splash.show();
    splash.raise();
    splash.activateWindow();

    a.processEvents();

    fprintf(stderr, "DEBUG: Splash shown\n");

    // Keep splash visible
    QEventLoop loop;
    QTimer::singleShot(
        SPLASH_DURATION_MS,
        &loop,
        &QEventLoop::quit
    );

    loop.exec();

    splash.close();

    fprintf(stderr, "DEBUG: Splash closed\n");

    // Apply Brusher theme
    BrusherTheme::apply(a);

    MainWindow w;
    w.show();

    fprintf(stderr, "DEBUG: MainWindow shown\n");

    return a.exec();
}