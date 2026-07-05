#pragma once

#include <QString>
#include <QColor>
#include <QUuid>
#include <optional>

class Note
{
public:
    Note()
        : m_id(QUuid::createUuid().toString(QUuid::WithoutBraces))
    {}

    static Note fromMarkdown(const QString &fullMarkdown);
    QString     toMarkdown() const;

    const QString &id() const
    {
        return m_id;
    }
    void setId(const QString &id)
    {
        m_id = id;
    }

    const QString &filename() const
    {
        return m_filename;
    }
    void setFilename(const QString &filename)
    {
        m_filename = filename;
    }

    const QString &content() const
    {
        return m_content;
    }
    void setContent(const QString &content)
    {
        m_content = content;
    }

    int width() const
    {
        return m_width;
    }
    void setWidth(int w)
    {
        m_width = w;
    }
    int height() const
    {
        return m_height;
    }
    void setHeight(int h)
    {
        m_height = h;
    }
    double xPct() const
    {
        return m_xPct;
    }
    void setXPct(double pct)
    {
        m_xPct = pct;
    }
    double yPct() const
    {
        return m_yPct;
    }
    void setYPct(double pct)
    {
        m_yPct = pct;
    }

    bool alwaysOnTop() const
    {
        return m_alwaysOnTop;
    }
    void setAlwaysOnTop(bool on)
    {
        m_alwaysOnTop = on;
    }

    bool locked() const
    {
        return m_locked;
    }
    void setLocked(bool locked)
    {
        m_locked = locked;
    }

    bool pinned() const
    {
        return m_pinned;
    }
    void setPinned(bool pinned)
    {
        m_pinned = pinned;
    }

    const std::optional<QColor> &bgColor() const
    {
        return m_bgColor;
    }
    void setBgColor(const std::optional<QColor> &c)
    {
        m_bgColor = c;
    }

    const std::optional<QColor> &textColor() const
    {
        return m_textColor;
    }
    void setTextColor(const std::optional<QColor> &c)
    {
        m_textColor = c;
    }

    const std::optional<QString> &fontFamily() const
    {
        return m_fontFamily;
    }
    void setFontFamily(const std::optional<QString> &f)
    {
        m_fontFamily = f;
    }

    const std::optional<int> &fontSize() const
    {
        return m_fontSize;
    }
    void setFontSize(const std::optional<int> &s)
    {
        m_fontSize = s;
    }

    const std::optional<int> &opacity() const
    {
        return m_opacity;
    }
    void setOpacity(const std::optional<int> &o)
    {
        m_opacity = o;
    }

    const std::optional<int> &iconSize() const
    {
        return m_iconSize;
    }
    void setIconSize(const std::optional<int> &s)
    {
        m_iconSize = s;
    }

    bool isDirty() const
    {
        return m_dirty;
    }
    void markDirty()
    {
        m_dirty = true;
    }
    void markSaved()
    {
        m_dirty = false;
    }

private:
    QString m_id;
    QString m_filename;
    QString m_content;
    int     m_width{300};
    int     m_height{300};
    double  m_xPct{-1.0};
    double  m_yPct{-1.0};
    bool    m_alwaysOnTop{true};
    bool    m_locked{false};
    bool    m_pinned{false};

    std::optional<QColor>  m_bgColor;
    std::optional<QColor>  m_textColor;
    std::optional<QString> m_fontFamily;
    std::optional<int>     m_fontSize;
    std::optional<int>     m_opacity;
    std::optional<int>     m_iconSize;

    bool m_dirty{false};
};
