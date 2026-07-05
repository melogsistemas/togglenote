#include "CornerButton.h"
#include "Icons.h"

CornerButton::CornerButton(const Config &cfg, QWidget *parent)
    : QToolButton(parent)
    , m_cfg(cfg)
{
    setAutoRaise(true);
    setCursor(Qt::PointingHandCursor);
    setCheckable(cfg.checkable);
    setFixedSize(33, 33);
    if (cfg.transparentForMouseEvents)
        setAttribute(Qt::WA_TransparentForMouseEvents);
    raise();
    updateIconSize(16);
    if (!cfg.checkedTooltip.isEmpty()) {
        connect(this, &QToolButton::toggled, this, [this]() {
            setToolTip(isChecked() ? m_cfg.checkedTooltip : m_cfg.uncheckedTooltip);
        });
    }
}

void CornerButton::updateIconSize(int baseIconSize)
{
    const QString path = Design::svgPath(m_cfg.icon);
    const int     sz   = baseIconSize + 6;

    const QColor gray30(115, 116, 120, 77);
    const QColor gray90(115, 116, 120, 230);
    const QColor accent30(255, 0, 0, 77);
    const QColor accent90(255, 0, 0, 230);

    QIcon icon;
    icon.addPixmap(Icons::renderSvg(path, gray30, sz), QIcon::Normal, QIcon::Off);
    icon.addPixmap(Icons::renderSvg(path, gray90, sz), QIcon::Active, QIcon::Off);
    if (m_cfg.checkable) {
        icon.addPixmap(Icons::renderSvg(path, accent30, sz), QIcon::Normal, QIcon::On);
        icon.addPixmap(Icons::renderSvg(path, accent90, sz), QIcon::Active, QIcon::On);
    }

    setIcon(icon);
    setIconSize(QSize(sz, sz));
    setFixedSize(sz + 4, sz + 4);
    setStyleSheet(QStringLiteral("QToolButton { background: transparent; border: none; }"));
    reposition();
}

void CornerButton::reposition()
{
    QWidget *p = parentWidget();
    if (!p)
        return;
    int y = 4;
    if (m_cfg.stackRow > 0)
        y += m_cfg.stackRow * (height() + 4);
    move(p->width() - width() - 4, y);
    raise();
}
