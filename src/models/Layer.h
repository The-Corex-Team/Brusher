#pragma once

#include <QString>
#include <QImage>

struct Layer {
    QString name;
    QImage image;
    bool visible = true;
    float opacity = 1.0f;
};
