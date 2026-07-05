#pragma once

#include <QString>
#include <QStringList>
#include <QSet>
#include <QColor>
#include <QHash>
#include <QKeySequence>
#include <QList>
#include "ButtonId.h"

namespace Defaults
{

QString notesDir();

} // namespace Defaults

struct ToolbarItem
{
    enum Type
    {
        Button,
        Separator
    };

    Type    type = Button;
    QString id; // action ID for Button, empty for Separator
    bool    visible = true;
};

class Settings
{
public:
    bool autostartNewNote() const
    {
        return m_autostartNewNote;
    }
    void setAutostartNewNote(bool v)
    {
        m_autostartNewNote = v;
    }

    bool startHidden() const
    {
        return m_startHidden;
    }
    void setStartHidden(bool v)
    {
        m_startHidden = v;
    }

    bool defaultOnTop() const
    {
        return m_defaultOnTop;
    }
    void setDefaultOnTop(bool v)
    {
        m_defaultOnTop = v;
    }

    bool autohideToolbar() const
    {
        return m_autohideToolbar;
    }
    void setAutohideToolbar(bool v)
    {
        m_autohideToolbar = v;
    }

    int autohideTimeout() const
    {
        return m_autohideTimeout;
    }
    void setAutohideTimeout(int v)
    {
        m_autohideTimeout = v;
    }

    const QColor &bgColor() const
    {
        return m_bgColor;
    }
    void setBgColor(const QColor &c)
    {
        m_bgColor = c;
    }

    const QColor &textColor() const
    {
        return m_textColor;
    }
    void setTextColor(const QColor &c)
    {
        m_textColor = c;
    }

    const QString &fontFamily() const
    {
        return m_fontFamily;
    }
    void setFontFamily(const QString &f)
    {
        m_fontFamily = f;
    }

    int fontSize() const
    {
        return m_fontSize;
    }
    void setFontSize(int v)
    {
        m_fontSize = v;
    }

    int iconSize() const
    {
        return m_iconSize;
    }
    void setIconSize(int v)
    {
        m_iconSize = v;
    }

    int opacity() const
    {
        return m_opacity;
    }
    void setOpacity(int v)
    {
        m_opacity = v;
    }

    int pinHoverOpacity() const
    {
        return m_pinHoverOpacity;
    }
    void setPinHoverOpacity(int v)
    {
        m_pinHoverOpacity = v;
    }

    int pinIdleOpacity() const
    {
        return m_pinIdleOpacity;
    }
    void setPinIdleOpacity(int v)
    {
        m_pinIdleOpacity = v;
    }

    const QStringList &customColors() const
    {
        return m_customColors;
    }
    void setCustomColors(const QStringList &c)
    {
        m_customColors = c;
    }

    const QList<ButtonId> &visibleButtons() const
    {
        return m_visibleButtons;
    }
    void setVisibleButtons(const QList<ButtonId> &b)
    {
        m_visibleButtons = b;
    }

    const QStringList &toolbarLayout() const
    {
        return m_toolbarLayout;
    }
    void setToolbarLayout(const QStringList &l)
    {
        m_toolbarLayout = l;
    }

    const QSet<QString> &toolbarLayoutVisibility() const
    {
        return m_toolbarLayoutVisibility;
    }
    void setToolbarLayoutVisibility(const QSet<QString> &v)
    {
        m_toolbarLayoutVisibility = v;
    }

    static QStringList defaultToolbarLayout()
    {
        return {
            QStringLiteral("note.new"),
            QStringLiteral("note.delete"),
            QStringLiteral("note.hide"),
            QStringLiteral("note.hideAll"),
            QStringLiteral("|"),
            QStringLiteral("editor.undo"),
            QStringLiteral("editor.redo"),
            QStringLiteral("|"),
            QStringLiteral("editor.bold"),
            QStringLiteral("editor.italic"),
            QStringLiteral("editor.underline"),
            QStringLiteral("editor.strikethrough"),
            QStringLiteral("editor.clearFormat"),
            QStringLiteral("|"),
            QStringLiteral("editor.cut"),
            QStringLiteral("editor.copy"),
            QStringLiteral("editor.paste"),
            QStringLiteral("|"),
            QStringLiteral("editor.find"),
            QStringLiteral("|"),
            QStringLiteral("editor.zoomIn"),
            QStringLiteral("editor.zoomOut"),
            QStringLiteral("|"),
            QStringLiteral(">>"),
            QStringLiteral("note.ontop"),
            QStringLiteral("note.prefs"),
        };
    }

    static QSet<QString> defaultToolbarLayoutVisibility()
    {
        return {
            QStringLiteral("note.new"),
            QStringLiteral("note.delete"),
            QStringLiteral("note.hide"),
            QStringLiteral("editor.undo"),
            QStringLiteral("editor.redo"),
            QStringLiteral("editor.bold"),
            QStringLiteral("editor.italic"),
            QStringLiteral("editor.underline"),
            QStringLiteral("editor.strikethrough"),
            QStringLiteral("editor.clearFormat"),
            QStringLiteral("note.ontop"),
            QStringLiteral("note.prefs"),
        };
    }

    const QHash<QString, QKeySequence> &keybindings() const
    {
        return m_keybindings;
    }
    void setKeybindings(const QHash<QString, QKeySequence> &k)
    {
        m_keybindings = k;
    }

    const QString &notesDir() const
    {
        return m_notesDir;
    }
    void setNotesDir(const QString &d)
    {
        m_notesDir = d;
    }

    bool forceXcbOnWayland() const
    {
        return m_forceXcbOnWayland;
    }
    void setForceXcbOnWayland(bool v)
    {
        m_forceXcbOnWayland = v;
    }

private:
    bool        m_autostartNewNote{false};
    bool        m_startHidden{false};
    bool        m_defaultOnTop{true};
    bool        m_forceXcbOnWayland{true};
    bool        m_autohideToolbar{true};
    int         m_autohideTimeout{1000};
    QColor      m_bgColor{QColor("#ffee99")};
    QColor      m_textColor{Qt::black};
    QString     m_fontFamily{QStringLiteral("Sans")};
    int         m_fontSize{12};
    int         m_iconSize{16};
    int         m_opacity{100};
    int         m_pinHoverOpacity{100};
    int         m_pinIdleOpacity{30};
    QStringList m_customColors{
        QStringLiteral("#ffee99"),
        QStringLiteral("#ccffcc"),
        QStringLiteral("#ccccff"),
        QStringLiteral("#ffcccc"),
        QStringLiteral("#ffdd99"),
        QStringLiteral("#ddccff"),
        QStringLiteral("#ccffee"),
        QStringLiteral("#ffccbb"),
        QStringLiteral("#99ccff"),
    };
    QList<ButtonId>              m_visibleButtons{ButtonId::New,
                                     ButtonId::Delete,
                                     ButtonId::Cut,
                                     ButtonId::Copy,
                                     ButtonId::Paste,
                                     ButtonId::Undo,
                                     ButtonId::Redo};
    QStringList                  m_toolbarLayout;
    QSet<QString>                m_toolbarLayoutVisibility;
    QHash<QString, QKeySequence> m_keybindings;
    QString                      m_notesDir{Defaults::notesDir()};
};

namespace FontSizeLimits
{
constexpr int kMin = 8;
constexpr int kMax = 72;
} // namespace FontSizeLimits

namespace IconLimits
{
constexpr int kMin        = 12;
constexpr int kMax        = 64;
constexpr int kStepButton = 18;
} // namespace IconLimits
