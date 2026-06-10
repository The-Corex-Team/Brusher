#pragma once

#include <QDockWidget>

class QToolButton;

class ToolPanel : public QDockWidget {
    Q_OBJECT
public:
    explicit ToolPanel(QWidget *parent = nullptr);

signals:
    void penToolSelected();
    void eraserToolSelected();
    void fillToolSelected();
    void eyedropperToolSelected();
    void lineToolSelected();

private slots:
    void onToolClicked();

private:
    QToolButton* createToolButton(const QIcon &icon, const QString &tooltip);

    QToolButton *m_penBtn;
    QToolButton *m_eraserBtn;
    QToolButton *m_fillBtn;
    QToolButton *m_eyedropperBtn;
    QToolButton *m_lineBtn;
};
