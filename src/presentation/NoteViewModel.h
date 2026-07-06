#pragma once

#include <QObject>
#include <QTimer>
#include <optional>
#include "Note.h"
#include "Settings.h"

class INoteService;

/// ViewModel for a single note. Wraps Note entity, resolves per-note overrides
/// against global defaults, manages debounced auto-save (500ms timer).
class NoteViewModel : public QObject
{
    Q_OBJECT
public:
    explicit NoteViewModel(const Note     &data,
                           INoteService   *service,
                           const Settings &globalDefaults,
                           QObject        *parent = nullptr);

    void updateContent(const QString &markdown);
    void updateGeometry(int width, int height, double xPct, double yPct);
    void save();
    void markDirty();
    void scheduleSave();

    // ── Property accessors (resolved: falls back to global defaults) ──
    QColor  bgColor() const;
    QColor  textColor() const;
    int     fontSize() const;
    QString fontFamily() const;
    int     opacity() const;
    QString content() const;
    bool    isLocked() const;
    bool    isPinned() const;
    bool    isAlwaysOnTop() const;
    QString filename() const;
    int     width() const;
    int     height() const;

    // ── Raw per-note data (bypasses global fallback) ──
    std::optional<int> iconSize() const;
    int                defaultToolbarIconSize() const
    {
        return m_globalDefaults.iconSize();
    }
    bool hasCustomToolbarIconSize() const
    {
        return m_data.iconSize().has_value();
    }

    // ── Setters ──
    void setFontSize(std::optional<int> size);
    void setFontFamily(std::optional<QString> family);
    void setTextColor(std::optional<QColor> color);
    void setBgColor(std::optional<QColor> color);
    void setOpacity(std::optional<int> pct);
    void setIconSize(std::optional<int> sz);
    void setLocked(bool locked);
    void setPinned(bool pinned);
    void setAlwaysOnTop(bool on);

    void            setGlobalDefaults(const Settings &s);
    const Settings &globalDefaults() const
    {
        return m_globalDefaults;
    }

    // ── Full snapshot for revert/copy ──
    const Note &snapshot() const
    {
        return m_data;
    }

signals:
    void styleChanged();

private:
    Note          m_data;
    INoteService *m_service;
    QTimer       *m_saveTimer;
    Settings      m_globalDefaults;
};
