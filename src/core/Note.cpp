#include "Note.h"
#include <QRegularExpression>


namespace
{
constexpr int kDefaultSize = 300;

const QString KEY_ID            = QStringLiteral("id");
const QString KEY_BG_COLOR      = QStringLiteral("bgColor");
const QString KEY_TEXT_COLOR    = QStringLiteral("textColor");
const QString KEY_WIDTH         = QStringLiteral("width");
const QString KEY_HEIGHT        = QStringLiteral("height");
const QString KEY_X_PCT         = QStringLiteral("xPct");
const QString KEY_Y_PCT         = QStringLiteral("yPct");
const QString KEY_ALWAYS_ON_TOP = QStringLiteral("alwaysOnTop");
const QString KEY_LOCKED        = QStringLiteral("locked");
const QString KEY_PINNED        = QStringLiteral("pinned");
const QString KEY_FONT_FAMILY   = QStringLiteral("fontFamily");
const QString KEY_FONT_SIZE     = QStringLiteral("fontSize");
const QString KEY_OPACITY       = QStringLiteral("opacity");
const QString KEY_ICON_SIZE     = QStringLiteral("iconSize");

void applyFrontMatter(Note &note, const QString &key, const QString &value)
{
    if (key == KEY_ID) {
        note.setId(value);
    }
    else if (key == KEY_BG_COLOR) {
        QColor color(value);
        if (color.isValid())
            note.setBgColor(color);
    }
    else if (key == KEY_TEXT_COLOR) {
        QColor color(value);
        if (color.isValid())
            note.setTextColor(color);
    }
    else if (key == KEY_WIDTH) {
        note.setWidth(value.toInt());
    }
    else if (key == KEY_HEIGHT) {
        note.setHeight(value.toInt());
    }
    else if (key == KEY_X_PCT) {
        note.setXPct(value.toDouble());
    }
    else if (key == KEY_Y_PCT) {
        note.setYPct(value.toDouble());
    }
    else if (key == KEY_ALWAYS_ON_TOP) {
        note.setAlwaysOnTop(value == QStringLiteral("true"));
    }
    else if (key == KEY_LOCKED) {
        note.setLocked(value == QStringLiteral("true"));
    }
    else if (key == KEY_PINNED) {
        note.setPinned(value == QStringLiteral("true"));
    }
    else if (key == KEY_FONT_FAMILY) {
        note.setFontFamily(value);
    }
    else if (key == KEY_FONT_SIZE) {
        note.setFontSize(value.toInt());
    }
    else if (key == KEY_OPACITY) {
        note.setOpacity(value.toInt());
    }
    else if (key == KEY_ICON_SIZE) {
        note.setIconSize(value.toInt());
    }
}

QString stripQuotes(const QString &value)
{
    if (value.startsWith(QLatin1Char('"')) && value.endsWith(QLatin1Char('"')))
        return value.mid(1, value.size() - 2);
    return value;
}
} // namespace

Note Note::fromMarkdown(const QString &fullMarkdown)
{
    static const QRegularExpression fmRe(QStringLiteral(R"(^---\s*\r?\n(.*?)\r?\n---\s*\r?\n)"),
                                         QRegularExpression::DotMatchesEverythingOption);
    QRegularExpressionMatch         match = fmRe.match(fullMarkdown);

    Note note;
    if (!match.hasMatch()) {
        note.setContent(fullMarkdown);
        return note;
    }

    for (const QString &line : match.captured(1).split(QLatin1Char('\n'))) {
        QString trimmed = line.trimmed();
        if (trimmed.isEmpty())
            continue;
        int colonPos = trimmed.indexOf(QStringLiteral(": "));
        if (colonPos <= 0)
            continue;
        applyFrontMatter(note, trimmed.left(colonPos), stripQuotes(trimmed.mid(colonPos + 2)));
    }

    note.setContent(fullMarkdown.mid(match.capturedEnd()));
    while (!note.content().isEmpty()
           && (note.content().at(0) == QLatin1Char('\n') || note.content().at(0) == QLatin1Char('\r')))
        note.setContent(note.content().mid(1));

    return note;
}

QString Note::toMarkdown() const
{
    QStringList lines;
    lines << QStringLiteral("---");

    auto write = [&](const QString &key, const QString &val) { lines << (key + QStringLiteral(": ") + val); };

    if (!m_id.isEmpty())
        write(KEY_ID, m_id);
    if (m_bgColor.has_value())
        write(KEY_BG_COLOR, m_bgColor->name());
    if (m_textColor.has_value())
        write(KEY_TEXT_COLOR, m_textColor->name());
    if (m_width != kDefaultSize)
        write(KEY_WIDTH, QString::number(m_width));
    if (m_height != kDefaultSize)
        write(KEY_HEIGHT, QString::number(m_height));
    if (m_xPct >= 0.0)
        write(KEY_X_PCT, QString::number(m_xPct));
    if (m_yPct >= 0.0)
        write(KEY_Y_PCT, QString::number(m_yPct));
    if (m_alwaysOnTop)
        write(KEY_ALWAYS_ON_TOP, QStringLiteral("true"));
    if (m_locked)
        write(KEY_LOCKED, QStringLiteral("true"));
    if (m_pinned)
        write(KEY_PINNED, QStringLiteral("true"));
    if (m_fontFamily.has_value())
        write(KEY_FONT_FAMILY, *m_fontFamily);
    if (m_fontSize.has_value())
        write(KEY_FONT_SIZE, QString::number(*m_fontSize));
    if (m_opacity.has_value())
        write(KEY_OPACITY, QString::number(*m_opacity));
    if (m_iconSize.has_value())
        write(KEY_ICON_SIZE, QString::number(*m_iconSize));

    lines << QStringLiteral("---");
    lines << QString();
    lines << m_content;

    return lines.join(QLatin1Char('\n'));
}
