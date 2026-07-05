#include "TrayIcon.h"

#include <QSystemTrayIcon>
#include <QMenu>
#include <QIcon>

TrayIcon::TrayIcon(QObject *parent)
    : QObject(parent)
{
    m_trayIcon = new QSystemTrayIcon(this);
    m_trayIcon->setIcon(QIcon(QStringLiteral(":/logo.svg")));
    m_trayIcon->setToolTip(QStringLiteral("togglenote"));

    m_trayMenu = new QMenu;
    m_trayMenu->addAction("New note", this, &TrayIcon::newNoteRequested);

    m_trayMenu->addSeparator();

    m_trayMenu->addAction("Show all", this, &TrayIcon::showAllRequested);
    m_trayMenu->addAction("Hide all", this, &TrayIcon::hideAllRequested);

    m_trayMenu->addSeparator();

    m_ghostModeAction
        = m_trayMenu->addAction("Ghost mode", this, [this](bool checked) { emit ghostModeToggled(checked); });
    m_ghostModeAction->setCheckable(true);
    m_ghostModeAction->setChecked(false);

    m_trayMenu->addSeparator();

    m_trayMenu->addAction("Preferences...", this, &TrayIcon::preferencesRequested);

    m_trayMenu->addSeparator();

    m_trayMenu->addAction("Quit", this, [this]() { emit quitRequested(); });

    m_trayIcon->setContextMenu(m_trayMenu);

    connect(m_trayIcon, &QSystemTrayIcon::activated, this, [this](QSystemTrayIcon::ActivationReason reason) {
#ifdef Q_OS_MACOS
        if (reason == QSystemTrayIcon::DoubleClick)
#else
        if (reason == QSystemTrayIcon::Trigger || reason == QSystemTrayIcon::DoubleClick)
#endif
            emit toggleRequested();
    });

    m_trayIcon->show();
}

TrayIcon::~TrayIcon()
{
    delete m_trayMenu;
}

void TrayIcon::setGhostModeChecked(bool on)
{
    m_ghostModeAction->blockSignals(true);
    m_ghostModeAction->setChecked(on);
    m_ghostModeAction->blockSignals(false);
}
