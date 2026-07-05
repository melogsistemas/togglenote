#pragma once

#include <QFrame>
#include <QIcon>
#include <QString>
#include <QList>
#include <functional>

struct MenuEntry
{
    QIcon                     icon;
    QString                   text;
    bool                      checkable = false;
    bool                      checked   = false;
    std::function<void(bool)> onClick;
};

class QVBoxLayout;

class ContextMenu : public QFrame
{
    Q_OBJECT
public:
    explicit ContextMenu(QWidget *parent = nullptr);

    int  addEntry(const MenuEntry &entry);
    void addSeparator();
    int  entryCount() const
    {
        return m_entries.size();
    }
    void popup(const QPoint &pos);

protected:
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void leaveEvent(QEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    struct EntryData
    {
        MenuEntry entry;
        bool      isSeparator = false;
        QWidget  *checkbox    = nullptr;
    };

    QVBoxLayout     *m_layout{nullptr};
    QList<EntryData> m_entries;

    QWidget *createRow(int index);
    QWidget *createSeparator();
    int      rowIndex(QWidget *row) const;
    void     activate(int index);
    void     setHover(QWidget *row);
    void     clearHover();
};
