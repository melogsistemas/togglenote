#pragma once

#include <QObject>
#include <QString>

class QLocalServer;
struct CommandLineArgs;

class CurrentProcessManager : public QObject
{
    Q_OBJECT
public:
    explicit CurrentProcessManager(const QString &serverName, QObject *parent = nullptr);

    bool tryConnect(const QStringList &args);
    void startServer();

signals:
    void argsReceived(const CommandLineArgs &args);

private:
    void handleConnection();

    QString       m_serverName;
    QLocalServer *m_server{nullptr};
};
