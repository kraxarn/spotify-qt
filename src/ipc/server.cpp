#include "server.hpp"
#include "lib/log.hpp"

Ipc::Server::Server(QObject *parent)
	: QLocalServer(parent)
{
	setMaxPendingConnections(1);
}

auto Ipc::Server::start() -> bool
{
	if (!listen(APP_NAME))
	{
		lib::log::error("Failed to start IPC server: {}",
			errorString().toStdString());
		return false;
	}

	QLocalServer::connect(this, &QLocalServer::newConnection,
		this, &Ipc::Server::onNewConnection);

	return true;
}

void Ipc::Server::onNewConnection()
{
	socket = nextPendingConnection();
	QLocalSocket::connect(socket, &QLocalSocket::disconnected,
		socket, &QLocalSocket::deleteLater);

	stream.setDevice(socket);

	QLocalSocket::connect(socket, &QLocalSocket::readyRead,
		this, &Ipc::Server::onReadyRead);
}

void Ipc::Server::onReadyRead()
{
	if (!stream.atEnd())
	{
		return;
	}

	QString data;
	stream >> data;
	Ipc::Server::onReadAll(data);

	QByteArray buffer;
	QDataStream out(&buffer, QIODevice::WriteOnly);
	out << QStringLiteral("ok");

	socket->write(buffer);
	socket->flush();
	socket->disconnectFromServer();
}

void Ipc::Server::onReadAll(const QString &data)
{
	lib::log::debug("IPC: {}", data.toStdString());
}
