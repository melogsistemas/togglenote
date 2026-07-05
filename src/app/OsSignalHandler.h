#pragma once

#include <QObject>
#include <QTimer>
#include <atomic>

class OsSignalHandler : public QObject
{
    Q_OBJECT
public:
    explicit OsSignalHandler(QObject *parent = nullptr);
    void start();

signals:
    void quitRequested();

private:
    void                    checkSignal();
    static void             sigHandler(int);
    static std::atomic<int> s_signalReceived;
    QTimer                 *m_checkTimer{nullptr};
};
