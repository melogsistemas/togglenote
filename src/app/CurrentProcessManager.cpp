#include "CurrentProcessManager.h"
#include "CommandLineArgs.h"

#include <QLocalServer>
#include <QLocalSocket>

namespace
{
constexpr int kConnectTimeoutMs = 500;
constexpr int kWriteTimeoutMs   = 2000;
constexpr int kReadTimeoutMs    = 2000;
} // namespace

CurrentProcessManager::CurrentProcessManager(const QString &serverName, QObject *parent)
    : QObject(parent)
    , m_serverName(serverName)
{}

bool CurrentProcessManager::tryConnect(const QStringList &args)
{
    QLocalSocket probe;
    probe.connectToServer(m_serverName);
    bool otherRunning = probe.waitForConnected(kConnectTimeoutMs);
    probe.disconnectFromServer();

    if (!otherRunning)
        return false;

    QLocalSocket client;
    client.connectToServer(m_serverName);
    if (client.waitForConnected(kConnectTimeoutMs)) {
        QByteArray data;
        for (int i = 1; i < args.size(); i++) {
            data.append(args[i].toUtf8());
            data.append('\0');
        }
        client.write(data);
        client.waitForBytesWritten(kWriteTimeoutMs);
        client.waitForDisconnected(kReadTimeoutMs);
    }
    return true;
}

void CurrentProcessManager::startServer()
{
    QLocalServer::removeServer(m_serverName);
    m_server = new QLocalServer(this);
    m_server->listen(m_serverName);
    connect(m_server, &QLocalServer::newConnection, this, &CurrentProcessManager::handleConnection);
}

void CurrentProcessManager::handleConnection()
{
    QLocalSocket *socket = m_server->nextPendingConnection();
    if (!socket)
        return;
    socket->waitForReadyRead(kReadTimeoutMs);
    QByteArray data = socket->readAll();
    socket->deleteLater();

    QStringList args;
    args << m_serverName;
    int start = 0;
    for (int i = 0; i < data.size(); i++) {
        if (data[i] == '\0') {
            args << QString::fromUtf8(data.mid(start, i - start));
            start = i + 1;
        }
    }
    if (start < data.size())
        args << QString::fromUtf8(data.mid(start));

    emit argsReceived(parseCommandLineArgs(args));
}
