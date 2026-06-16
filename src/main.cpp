#include "ui/MainWindow.h"
#include "ui/BrusherTheme.h"
#include <QApplication>
#include <QWidget>
#include <QSvgRenderer>
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

// Frameless splash window that renders an SVG from resources.
class SplashWindow : public QWidget {
public:
    SplashWindow(const QString &svgPath, int width, int height, QWidget *parent = nullptr)
        : QWidget(parent), m_renderer(svgPath), m_width(width), m_height(height)
    {
        setFixedSize(m_width, m_height);
        setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Dialog);
        setAttribute(Qt::WA_TranslucentBackground);
    }

protected:
    void paintEvent(QPaintEvent *) override {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        p.setRenderHint(QPainter::SmoothPixmapTransform);
        m_renderer.render(&p);
    }

private:
    QSvgRenderer m_renderer;
    int m_width, m_height;
};

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    fprintf(stderr, "DEBUG: App created\n");

    // Create and show splash window BEFORE initializing the main app theme.
    SplashWindow splash(QStringLiteral(":/src/icons/Brusher_Splashscreen.svg"), SPLASH_WIDTH, SPLASH_HEIGHT);
    fprintf(stderr, "DEBUG: SplashWindow created with size %dx%d\n", SPLASH_WIDTH, SPLASH_HEIGHT);

    // Center on the primary screen.
    QScreen *screen = QGuiApplication::primaryScreen();
    fprintf(stderr, "DEBUG: Primary screen = %p\n", screen);
    if (screen) {
        QRect g = screen->availableGeometry();
        QPoint center = g.center() - splash.rect().center();
        fprintf(stderr, "DEBUG: Screen geometry: %d,%d %dx%d\n", g.x(), g.y(), g.width(), g.height());
        fprintf(stderr, "DEBUG: Moving splash to: %d,%d\n", center.x(), center.y());
        splash.move(center);
    }

    splash.show();
    fprintf(stderr, "DEBUG: Splash shown\n");
    splash.raise();
    splash.activateWindow();
    a.processEvents();
    fprintf(stderr, "DEBUG: Splash raised and window activated\n");

    // Keep splash visible for configured duration.
    fprintf(stderr, "DEBUG: Waiting %d ms...\n", SPLASH_DURATION_MS);
    QEventLoop loop;
    QTimer::singleShot(SPLASH_DURATION_MS, &loop, &QEventLoop::quit);
    loop.exec();
    fprintf(stderr, "DEBUG: Duration passed\n");

    splash.close();

    // Apply the cross-platform Brusher dark theme.
    BrusherTheme::apply(a);
    fprintf(stderr, "DEBUG: Theme applied\n");

    MainWindow w;
    w.show();
    fprintf(stderr, "DEBUG: MainWindow shown\n");

    return a.exec();
}
