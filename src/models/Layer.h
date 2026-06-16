#pragma once

#include <QString>
#include <QImage>

enum class BlendMode {
    Normal,
    Multiply,
    Screen,
    Overlay,
    Darken,
    Lighten
};

struct Layer {
    QString name;
    QImage image;
    bool visible = true;
    float opacity = 1.0f;
    BlendMode blendMode = BlendMode::Normal;
};
