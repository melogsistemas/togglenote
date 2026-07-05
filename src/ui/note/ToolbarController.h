#pragma once

#include <QObject>
#include <QMap>
#include <QList>
#include <QSet>
#include <QString>
#include "ActionId.h"
#include "ToolbarWidget.h"
#include "ToolbarButton.h"
#include "ButtonId.h"

class NoteEditor;
class SearchBar;
class ISettingsProvider;
class ContextMenu;
struct EditorState;

class ToolbarController : public QObject
{
    Q_OBJECT
public:
    ToolbarController(NoteEditor        *editor,
                      SearchBar         *searchBar,
                      ToolbarWidget     *toolbar,
                      ISettingsProvider *settingsProvider,
                      QObject           *parent = nullptr);

    void init();

    QString buttonId(ButtonId buttonType) const
    {
        return m_buttonIds.value(buttonType);
    }
    void syncToolbarState(const EditorState &editorState);

    void           applyLayoutVisibility();
    QSet<ActionId> layoutVisibility() const;
    void           saveLayoutVisibility();

    ContextMenu *createContextMenu(QWidget *parent);
    void         reloadToolbarLayout();
    void         reloadToolbarLayout(const QStringList &layout, const QSet<QString> &vis);

public slots:
    void toggleSearch();

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

private:
    QList<QList<ToolbarButton>> buildToolbarSpec();
    void                        onSearchRequested(const QString &text, bool forward);
    void                        onSearchClosed();

    ToolbarWidget              *m_toolbar;
    NoteEditor                 *m_editor;
    SearchBar                  *m_searchBar;
    ISettingsProvider          *m_settingsProvider;
    QMap<ButtonId, QString>     m_buttonIds;
    QMap<QString, ActionId>     m_widgetIdToActionId;
    QList<QList<ToolbarButton>> m_specs;
    QStringList                 m_toolbarLayout;
    QSet<QString>               m_toolbarLayoutVis;
    QSet<ActionId>              m_layoutItemIds;
};
