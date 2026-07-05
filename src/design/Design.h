#pragma once
#include <QString>
#include <QWidget>
#include <QFrame>
#include <QStyle>
#include "Icon.h"

namespace Design
{

// ── Color palette (for QPainter / non-QSS contexts) ──
inline const QString Background         = QStringLiteral("#232629");
inline const QString Panel              = QStringLiteral("#31363b");
inline const QString Foreground         = QStringLiteral("#eff0f1");
inline const QString ForegroundDisabled = QStringLiteral("#737478");
inline const QString Separator          = QStringLiteral("#62686e");
inline const QString Border             = QStringLiteral("#454b50");
inline const QString Accent             = QStringLiteral("#3daee9");
inline const QString Hover              = QStringLiteral("#3a3f44");
inline const QString Selected           = QStringLiteral("#2a67a0");
inline const QString Danger             = QStringLiteral("#da4453");
inline const QString DangerDark         = QStringLiteral("#2a1b1d");

// ── Common widget helpers ──
inline QString buttonStyle(const QString &padding = QStringLiteral("4px 8px"), const QString &hoverColor = Accent)
{
    return QStringLiteral("QPushButton { padding: %1; background: %2; border: 1px solid %3;"
                          "  border-radius: 3px; color: %4; }"
                          "QPushButton:hover { border-color: %5; }")
        .arg(padding, Background, Border, Foreground, hoverColor);
}

inline QString iconButtonStyle(const QString &hoverColor = Foreground)
{
    return QStringLiteral("QPushButton { padding: 2px; background: %1; border: 1px solid %2;"
                          "  border-radius: 3px; }"
                          "QPushButton:hover { border-color: %3; }")
        .arg(Background, Border, hoverColor);
}

inline QFrame *horizontalLine(const QString &color = Separator)
{
    auto *line = new QFrame;
    line->setFixedHeight(1);
    line->setStyleSheet(QStringLiteral("background: %1; border: none;").arg(color));
    return line;
}

inline void refreshStyleProperty(QWidget *w, const QByteArray &prop, const QVariant &value)
{
    w->setProperty(prop.constData(), value);
    w->style()->unpolish(w);
    w->style()->polish(w);
}

// ── Icon resource path mapping ──
QString svgPath(Icon id);

} // namespace Design
