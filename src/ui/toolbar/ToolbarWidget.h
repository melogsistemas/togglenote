#pragma once

#include <QWidget>
#include <QList>
#include <QMap>
#include <QSet>
#include <QIcon>
#include <QString>
#include "ToolbarButton.h"

class QToolButton;
class QResizeEvent;

class ToolbarWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ToolbarWidget(const QList<QList<ToolbarButton>> &groups, QWidget *parent = nullptr);

    void setButtons(const QList<QList<ToolbarButton>> &groups);
    void setIconSize(int size);
    int  iconSize() const
    {
        return m_iconSize;
    }
    bool isRebuilding() const
    {
        return m_rebuilding;
    }

    void setButtonVisible(const QString &id, bool visible);
    bool isButtonVisible(const QString &id) const;
    void setButtonChecked(const QString &id, bool checked);
    bool isButtonChecked(const QString &id) const;
    void setButtonEnabled(const QString &id, bool enabled);

signals:
    void visibleButtonsChanged();

protected:
    void  resizeEvent(QResizeEvent *event) override;
    bool  eventFilter(QObject *obj, QEvent *event) override;
    QSize minimumSizeHint() const override;

private:
    struct GroupWidgets
    {
        QWidget             *separator = nullptr;
        QList<QToolButton *> buttons;
    };

    void         rebuild();
    void         applyVisibility();
    void         updateOverflow();
    QWidget     *createToolbarSeparator();
    QToolButton *createToolbarButton(const ToolbarButton &spec);

    QList<QList<ToolbarButton>>  m_groups;
    QMap<QString, QToolButton *> m_buttons;
    QList<GroupWidgets>          m_groupWidgets;
    QSet<QString>                m_hiddenButtons;
    QSet<QString>                m_overflowHidden;
    int                          m_iconSize{16};
    bool                         m_rebuilding{false};
};
