#include "TextTool.h"
#include "../canvas/CanvasWidget.h"
#include <QInputDialog>
#include <QPainter>
#include <QFont>

void TextTool::mousePressEvent(QMouseEvent *event, CanvasWidget *canvas, QImage *image)
{
    if (event->button() != Qt::LeftButton) {
        return;
    }

    bool ok = false;
    const QString text = QInputDialog::getText(
        canvas,
        QObject::tr("Enter Text"),
        QObject::tr("Text:"),
        QLineEdit::Normal,
        QString(),
        &ok);

    if (!ok || text.isEmpty()) {
        return;
    }

    canvas->pushHistoryState("Add Text");

    QFont font(canvas->textFontFamily(), canvas->textFontSize());
    font.setBold(m_bold);
    font.setItalic(m_italic);

    QPainter painter(image);
    painter.setFont(font);
    painter.setPen(canvas->brushColor());
    painter.drawText(event->position().toPoint(), text);

    canvas->update();
}

void TextTool::mouseMoveEvent(QMouseEvent *event, CanvasWidget *canvas, QImage *image)
{
    Q_UNUSED(event);
    Q_UNUSED(canvas);
    Q_UNUSED(image);
}

void TextTool::mouseReleaseEvent(QMouseEvent *event, CanvasWidget *canvas, QImage *image)
{
    Q_UNUSED(event);
    Q_UNUSED(canvas);
    Q_UNUSED(image);
}
