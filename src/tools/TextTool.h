#pragma once

#include "Tool.h"
#include <QPoint>
#include <QString>

class TextTool : public Tool {
public:
    void mousePressEvent(QMouseEvent *event, CanvasWidget *canvas, QImage *image) override;
    void mouseMoveEvent(QMouseEvent *event, CanvasWidget *canvas, QImage *image) override;
    void mouseReleaseEvent(QMouseEvent *event, CanvasWidget *canvas, QImage *image) override;

    void setFontFamily(const QString &family) { m_fontFamily = family; }
    void setFontSize(int size) { m_fontSize = size; }
    void setBold(bool bold) { m_bold = bold; }
    void setItalic(bool italic) { m_italic = italic; }

private:
    QString m_fontFamily = "Sans Serif";
    int m_fontSize = 24;
    bool m_bold = false;
    bool m_italic = false;
};
