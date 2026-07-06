#pragma once

#include <QMap>
#include <QList>
#include <QSet>
#include <QString>
#include "IToolbarController.h"
#include "ToolbarWidget.h"
#include "ToolbarButton.h"

class NoteEditor;
class SearchBar;
class ISettingsProvider;

/// Controls toolbar layout, button state, search bar integration, and context menu creation.
/// Relies on ToolbarWidget for rendering and ISettingsProvider for persisted layout.
class ToolbarController : public IToolbarController
{
    Q_OBJECT
public:
    ToolbarController(NoteEditor        *editor,
                      SearchBar         *searchBar,
                      ToolbarWidget     *toolbar,
                      ISettingsProvider *settingsProvider,
                      QObject           *parent = nullptr);

    void init() override;

    QString buttonId(ButtonId buttonType) const override
    {
        return m_buttonIds.value(buttonType);
    }
    void syncToolbarState(const EditorState &editorState) override;

    void           applyLayoutVisibility();
    QSet<ActionId> layoutVisibility() const;
    void           saveLayoutVisibility();

    ContextMenu *createContextMenu(QWidget *parent) override;
    void         reloadToolbarLayout() override;
    void         reloadToolbarLayout(const QStringList &layout, const QSet<QString> &vis) override;

public slots:
    void toggleSearch() override;

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
