#include "ImageFilters.h"
#include <QPainter>
#include <algorithm>

namespace ImageFilters {

static QRgb clampPixel(int r, int g, int b, int a)
{
    return qRgba(std::clamp(r, 0, 255), std::clamp(g, 0, 255), std::clamp(b, 0, 255), a);
}

QImage applyBlur(const QImage &source, int radius)
{
    if (source.isNull() || radius <= 0) {
        return source.copy();
    }

    QImage src = source.convertToFormat(QImage::Format_ARGB32);
    QImage result(src.size(), QImage::Format_ARGB32);
    const int w = src.width();
    const int h = src.height();
    const int r = radius;

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            qint64 sumR = 0, sumG = 0, sumB = 0, sumA = 0;
            int count = 0;

            for (int dy = -r; dy <= r; ++dy) {
                for (int dx = -r; dx <= r; ++dx) {
                    const int sx = x + dx;
                    const int sy = y + dy;
                    if (sx >= 0 && sx < w && sy >= 0 && sy < h) {
                        const QRgb pixel = src.pixel(sx, sy);
                        sumR += qRed(pixel);
                        sumG += qGreen(pixel);
                        sumB += qBlue(pixel);
                        sumA += qAlpha(pixel);
                        count++;
                    }
                }
            }

            result.setPixel(x, y, clampPixel(
                static_cast<int>(sumR / count),
                static_cast<int>(sumG / count),
                static_cast<int>(sumB / count),
                static_cast<int>(sumA / count)));
        }
    }

    return result;
}

QImage applySharpen(const QImage &source, float amount)
{
    if (source.isNull()) {
        return source.copy();
    }

    QImage src = source.convertToFormat(QImage::Format_ARGB32);
    QImage result = src.copy();
    const int w = src.width();
    const int h = src.height();

    const int kernel[3][3] = {
        {0, -1, 0},
        {-1, 5, -1},
        {0, -1, 0}
    };

    for (int y = 1; y < h - 1; ++y) {
        for (int x = 1; x < w - 1; ++x) {
            int sumR = 0, sumG = 0, sumB = 0;

            for (int ky = -1; ky <= 1; ++ky) {
                for (int kx = -1; kx <= 1; ++kx) {
                    const QRgb pixel = src.pixel(x + kx, y + ky);
                    const int weight = kernel[ky + 1][kx + 1];
                    sumR += qRed(pixel) * weight;
                    sumG += qGreen(pixel) * weight;
                    sumB += qBlue(pixel) * weight;
                }
            }

            const QRgb orig = src.pixel(x, y);
            const int r = qRed(orig) + static_cast<int>((sumR - qRed(orig)) * amount);
            const int g = qGreen(orig) + static_cast<int>((sumG - qGreen(orig)) * amount);
            const int b = qBlue(orig) + static_cast<int>((sumB - qBlue(orig)) * amount);
            result.setPixel(x, y, clampPixel(r, g, b, qAlpha(orig)));
        }
    }

    return result;
}

QImage applyToRegion(const QImage &source, const QRect &region, const QImage &filtered)
{
    QImage result = source.copy();
    QPainter painter(&result);
    painter.drawImage(region.topLeft(), filtered);
    return result;
}

} // namespace ImageFilters
