#pragma once

#include <QObject>
#include <QTimer>

class AutohideController : public QObject
{
    Q_OBJECT
public:
    explicit AutohideController(QObject *parent = nullptr);

    bool isToolbarVisible() const
    {
        return m_toolbarVisible;
    }
    bool isAutohideEnabled() const
    {
        return m_autohide;
    }
    bool isUserHidden() const
    {
        return m_userHiddenToolbar;
    }

    void setEnabled(bool on);
    void setTimeout(int ms);
    void showToolbar();
    void hideToolbar();
    void startHideTimer();
    void stopHideTimer();
    void setUserHidden(bool hidden)
    {
        m_userHiddenToolbar = hidden;
    }

signals:
    void toolbarShown();
    void toolbarHidden();

private:
    QTimer *m_hideTimer{nullptr};
    bool    m_toolbarVisible{false};
    bool    m_autohide{true};
    bool    m_userHiddenToolbar{false};
};
