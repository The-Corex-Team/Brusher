#include "ui/MainWindow.h"
#include "ui/BrusherTheme.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    
    // Apply the cross-platform Brusher dark theme.
    // This forces Qt Fusion style + QPalette + comprehensive QSS
    // so the app looks identical on Linux, Windows, and macOS.
    BrusherTheme::apply(a);
    
    MainWindow w;
    w.show();
    return a.exec();
}
