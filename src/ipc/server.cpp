#include "server.hpp"
#include "lib/log.hpp"

Ipc::Server::Server(QObject *parent)
	: QLocalServer(parent)
{
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
	QByteArray buffer;
	QDataStream out(&buffer, QIODevice::WriteOnly);

	out << QStringLiteral("ok");

	auto *socket = nextPendingConnection();
	QLocalSocket::connect(socket, &QLocalSocket::disconnected,
		socket, &QLocalSocket::deleteLater);

	const auto data = QString(socket->readAll());
	lib::log::debug("IPC: {}", data.toStdString());

	socket->write(buffer);
	socket->flush();
	socket->disconnectFromServer();
}
