#pragma once

#include <QDockWidget>
#include <QColor>

class QToolButton;
class QVBoxLayout;
class QLabel;

class ToolPanel : public QDockWidget {
    Q_OBJECT
public:
    explicit ToolPanel(QWidget *parent = nullptr);

    void updateSwatches(const QColor &fg, const QColor &bg);

signals:
    void toolSelected(int toolType);
    void swapColors();

private slots:
    void onToolClicked();
    void onSwapColorsClicked();

private:
    QToolButton* createToolButton(const QIcon &icon, const QString &tooltip);
    void setupForegroundBackground();

    QVBoxLayout *m_layout;

    QToolButton *m_moveBtn;
    QToolButton *m_rectSelectBtn;
    QToolButton *m_ellipseSelectBtn;
    QToolButton *m_lassoSelectBtn;
    QToolButton *m_penBtn;
    QToolButton *m_eraserBtn;
    QToolButton *m_fillBtn;
    QToolButton *m_eyedropperBtn;
    QToolButton *m_lineBtn;
    QToolButton *m_textBtn;
    QToolButton *m_zoomBtn;
    QToolButton *m_panBtn;

    QLabel *m_fgColorLabel;
    QLabel *m_bgColorLabel;
    QToolButton *m_swapColorsBtn;
};
