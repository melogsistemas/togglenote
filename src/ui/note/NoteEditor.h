#pragma once

#include <QTextEdit>
#include <QColor>
#include <QMap>
#include "ActionId.h"

struct EditorState
{
    bool bold{false};
    bool italic{false};
    bool underline{false};
    bool strike{false};
    int  fontSize{12};
    bool hasSelection{false};
    bool undoAvailable{false};
    bool redoAvailable{false};
    bool pasteAvailable{false};
};

class NoteEditor : public QTextEdit
{
    Q_OBJECT
public:
    explicit NoteEditor(QWidget *parent = nullptr);

    void toggleBold();
    void toggleItalic();
    void toggleUnderline();
    void toggleStrikethrough();
    void clearFormatting();

    void setLocked(bool locked);
    bool isLocked() const
    {
        return m_locked;
    }
    bool isInTempEdit() const
    {
        return m_tempEdit;
    }

    void setBaseFontSize(int size);
    void setBaseFontFamily(const QString &family);
    void setBaseTextColor(const QColor &color);
    void setBaseBgColor(const QColor &color);

    EditorState currentState() const;
    bool        hasSelection() const
    {
        return textCursor().hasSelection();
    }

    void setAlwaysOnTop(bool on);
    bool isAlwaysOnTop() const
    {
        return m_alwaysOnTop;
    }
    void setToolbarVisible(bool on);
    bool isToolbarVisible() const
    {
        return m_toolbarVisible;
    }
    void setHotkey(ActionId actionId, const QKeySequence &seq);
    void moveCursorToEnd();

signals:
    void stateChanged(const EditorState &state);
    void createNoteRequested();
    void deleteNoteRequested();
    void alwaysOnTopRequested();
    void toolbarToggleRequested();
    void noteSettingsRequested(const QPoint &pos);
    void preferencesRequested();
    void findRequested();
    void hideNoteRequested();
    void hideAllNotesRequested();

protected:
    bool event(QEvent *event) override;
    void contextMenuEvent(QContextMenuEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;

private:
    template<typename Predicate>
    bool selectionAllHave(Predicate predicate) const
    {
        QTextCursor cursor = textCursor();
        if (!cursor.hasSelection())
            return predicate(cursor.charFormat());
        int         start = cursor.selectionStart();
        int         end   = cursor.selectionEnd();
        QTextCursor tmp(document());
        int         docLen = document()->characterCount();
        for (int pos = start; pos < end; ++pos) {
            if (pos + 1 >= docLen)
                break;
            tmp.setPosition(pos + 1);
            if (!predicate(tmp.charFormat()))
                return false;
        }
        return true;
    }

    template<typename Predicate, typename Setter>
    void toggleProperty(Predicate predicate, Setter setter)
    {
        QTextCursor cursor = textCursor();

        if (cursor.hasSelection()) {
            bool            all = selectionAllHave(predicate);
            QTextCharFormat fmt;
            setter(fmt, !all);
            mergeCurrentCharFormat(fmt);
            setFocus();
            return;
        }

        bool isOnFormatted = predicate(cursor.charFormat());

        QTextCursor wordCursor = cursor;
        wordCursor.select(QTextCursor::WordUnderCursor);

        QTextCharFormat fmt;
        setter(fmt, !isOnFormatted);
        wordCursor.mergeCharFormat(fmt);

        setTextCursor(cursor);
        setFocus();
    }

    void enterTempEdit();
    void exitTempEdit();
    void emitState();
    void updateStylesheet();

    void filterStandardMenuActions(QMenu *menu, bool hasSelection);
    void buildNoteActionsMenu(QMenu *menu, const QPoint &globalPos);
    void buildFormattingActionsMenu(QMenu *menu);
    void applyHotkeyToAction(QAction *action, ActionId actionId);

    bool                         m_locked{false};
    bool                         m_tempEdit{false};
    bool                         m_alwaysOnTop{false};
    bool                         m_toolbarVisible{false};
    QMap<ActionId, QKeySequence> m_hotkeys;
    int                          m_pressPos{-1};
    QColor                       m_baseTextColor;
    QColor                       m_baseBgColor;
};
