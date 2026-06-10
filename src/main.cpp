#include "ui/MainWindow.h"
#include <QApplication>
#include <QStyleFactory>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    
    // Set modern fusion style as base
    a.setStyle(QStyleFactory::create("Fusion"));
    
    // Apply Dark Theme QSS
    QString darkTheme = R"(
        QWidget {
            background-color: #2b2b2b;
            color: #d1d1d1;
            font-family: "Inter", "Segoe UI", sans-serif;
            font-size: 10pt;
        }
        QMainWindow::separator {
            background: #3b3b3b;
            width: 2px;
            height: 2px;
        }
        QMenuBar {
            background-color: #242424;
            color: #d1d1d1;
            border-bottom: 1px solid #1e1e1e;
        }
        QMenuBar::item:selected {
            background-color: #3b3b3b;
            border-radius: 4px;
        }
        QMenu {
            background-color: #2b2b2b;
            border: 1px solid #1e1e1e;
        }
        QMenu::item:selected {
            background-color: #3daee9;
            color: white;
        }
        QDockWidget {
            titlebar-close-icon: url(close.png);
            titlebar-normal-icon: url(float.png);
            border: 1px solid #1e1e1e;
        }
        QDockWidget::title {
            background: #242424;
            padding-left: 5px;
            padding-top: 3px;
            padding-bottom: 3px;
        }
        QPushButton, QToolButton {
            background-color: #3b3b3b;
            border: 1px solid #1e1e1e;
            border-radius: 4px;
            padding: 5px 15px;
        }
        QPushButton:hover, QToolButton:hover {
            background-color: #4a4a4a;
            border-color: #555555;
        }
        QPushButton:pressed, QToolButton:checked {
            background-color: #3daee9;
            color: white;
            border-color: #2a8cc4;
        }
        QListWidget {
            background-color: #1e1e1e;
            border: 1px solid #3b3b3b;
            border-radius: 4px;
        }
        QListWidget::item {
            padding: 4px;
        }
        QListWidget::item:selected {
            background-color: #3daee9;
            color: white;
        }
        QSpinBox, QSlider {
            background-color: #1e1e1e;
            border: 1px solid #3b3b3b;
            border-radius: 4px;
            padding: 2px;
        }
    )";
    
    a.setStyleSheet(darkTheme);
    
    MainWindow w;
    w.show();
    return a.exec();
}
