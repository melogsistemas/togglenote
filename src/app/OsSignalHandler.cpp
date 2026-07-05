#include "OsSignalHandler.h"
#include <signal.h>

std::atomic<int> OsSignalHandler::s_signalReceived{0};

void OsSignalHandler::sigHandler(int)
{
    s_signalReceived.store(1, std::memory_order_seq_cst);
}

OsSignalHandler::OsSignalHandler(QObject *parent)
    : QObject(parent)
{}

void OsSignalHandler::start()
{
#ifndef Q_OS_WIN
    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags   = 0;
    sa.sa_handler = sigHandler;
    sigaction(SIGINT, &sa, nullptr);
    sigaction(SIGTERM, &sa, nullptr);
#endif

    m_checkTimer                   = new QTimer(this);
    constexpr int kCheckIntervalMs = 100;
    m_checkTimer->setInterval(kCheckIntervalMs);
    connect(m_checkTimer, &QTimer::timeout, this, &OsSignalHandler::checkSignal);
    m_checkTimer->start();
}

void OsSignalHandler::checkSignal()
{
    if (!s_signalReceived.load(std::memory_order_seq_cst))
        return;
    s_signalReceived.store(0, std::memory_order_seq_cst);
    emit quitRequested();
}
