#include "server.hpp"

Ipc::Server::Server(QObject *parent)
	: QLocalServer(parent)
{
}

auto Ipc::Server::start() -> bool
{
	if (!listen(APP_NAME))
	{
		return false;
	}

	QLocalServer::connect(this, &QLocalServer::newConnection,
		this, &Ipc::Server::onNewConnection);

	return true;
}

void Ipc::Server::onNewConnection()
{
	QByteArray buffer;
	QDataStream out(&buffer, QIODevice::WriteOnly);

	out << "pong"; // TODO

	auto *socket = nextPendingConnection();
	QLocalSocket::connect(socket, &QLocalSocket::disconnected,
		socket, &QLocalSocket::deleteLater);

	socket->write(buffer);
	socket->flush();
	socket->disconnectFromServer();
}
