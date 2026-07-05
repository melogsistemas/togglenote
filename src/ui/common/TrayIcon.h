#pragma once

#include <QObject>

class QSystemTrayIcon;
class QMenu;
class QIcon;
class QAction;

class TrayIcon : public QObject
{
    Q_OBJECT
public:
    explicit TrayIcon(QObject *parent = nullptr);
    ~TrayIcon() override;

    void setGhostModeChecked(bool on);

signals:
    void newNoteRequested();
    void showAllRequested();
    void hideAllRequested();
    void toggleRequested();
    void ghostModeToggled(bool enabled);
    void preferencesRequested();
    void quitRequested();

private:
    QSystemTrayIcon *m_trayIcon{nullptr};
    QMenu           *m_trayMenu{nullptr};
    QAction         *m_ghostModeAction{nullptr};
};
