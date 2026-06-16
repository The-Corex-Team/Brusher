#pragma once

#include <QImage>
#include <QRect>

namespace ImageFilters {

QImage applyBlur(const QImage &source, int radius);
QImage applySharpen(const QImage &source, float amount = 1.0f);
QImage applyToRegion(const QImage &source, const QRect &region, const QImage &filtered);

} // namespace ImageFilters
