#include "Icons.h"

#include <QSvgRenderer>
#include <QPainter>
#include <QPixmap>
#include <QHash>

namespace
{

quint64 s_themeVersion = 0;

struct IconCacheKey
{
    Design::Icon icon;
    int          size;
    quint64      themeVersion;
};

bool operator==(const IconCacheKey &a, const IconCacheKey &b)
{
    return a.icon == b.icon && a.size == b.size && a.themeVersion == b.themeVersion;
}

size_t qHash(const IconCacheKey &key, size_t seed = 0)
{
    return ::qHash(static_cast<int>(key.icon), seed) ^ ::qHash(key.size, seed) ^ ::qHash(key.themeVersion, seed);
}

QHash<IconCacheKey, QIcon> &iconCache()
{
    static QHash<IconCacheKey, QIcon> cache;
    return cache;
}

} // namespace

QPixmap Icons::renderSvg(const QString &path, const QColor &color, int size)
{
    QSvgRenderer renderer(path);
    QPixmap      pixmap(size, size);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    renderer.render(&painter);
    painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    painter.fillRect(pixmap.rect(), color);
    painter.end();
    return pixmap;
}

QIcon Icons::icon(Design::Icon id, int size)
{
    auto &cache = iconCache();
    const IconCacheKey key{id, size, s_themeVersion};
    auto it = cache.find(key);


    if (it != cache.end())
        return *it;

    const auto path = Design::svgPath(id);
    QIcon      icon;
    icon.addPixmap(renderSvg(path, QColor(Design::Foreground), size), QIcon::Normal);
    icon.addPixmap(renderSvg(path, QColor(Design::ForegroundDisabled), size), QIcon::Disabled);
    icon.addPixmap(renderSvg(path, QColor(Design::Foreground), size), QIcon::Selected);
    icon.addPixmap(renderSvg(path, QColor(Design::Foreground), size), QIcon::Active);

    cache.insert(key, std::move(icon));
    return cache[key];
}
