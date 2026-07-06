#pragma once

#include <QObject>
#include <QSet>
#include <QString>
#include <QStringList>
#include "ActionId.h"
#include "ButtonId.h"

struct EditorState;
class ContextMenu;
class QWidget;

/// Controller interface for toolbar layout, state, and context menu creation.
class IToolbarController : public QObject
{
    Q_OBJECT
public:
    using QObject::QObject;
    ~IToolbarController() override = default;

    virtual void         init()                                                                   = 0;
    virtual ContextMenu *createContextMenu(QWidget *parent)                                       = 0;
    virtual void         reloadToolbarLayout()                                                    = 0;
    virtual void         reloadToolbarLayout(const QStringList &layout, const QSet<QString> &vis) = 0;
    virtual void         toggleSearch()                                                           = 0;
    virtual QString      buttonId(ButtonId buttonType) const                                      = 0;
    virtual void         syncToolbarState(const struct EditorState &editorState)                  = 0;

signals:
    void newNoteRequested();
    void deleteNoteRequested();
    void alwaysOnTopToggled(bool on);
    void ghostModeToggled(bool on);
    void zoomInRequested();
    void zoomOutRequested();
    void preferencesRequested();
    void hideNoteRequested();
    void hideAllNotesRequested();
};
