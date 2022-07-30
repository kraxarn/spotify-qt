#include "spotify/authserver.hpp"

spt::AuthServer::AuthServer(lib::settings &settings, QObject *parent)
	: QTcpServer(parent),
	settings(settings),
	auth(settings, this)
{
	QTcpServer::connect(this, &QTcpServer::newConnection,
		this, &spt::AuthServer::onNewConnection);
}

auto spt::AuthServer::listen() -> bool
{
	return QTcpServer::listen(QHostAddress::LocalHost, serverPort);
}

auto spt::AuthServer::redirectUrl() -> QString
{
	return QString("http://localhost:%1").arg(serverPort);
}

void spt::AuthServer::onNewConnection()
{
	// Read
	auto *socket = nextPendingConnection();
	socket->waitForReadyRead();
	auto response = QString(socket->readAll());

	// Unrelated request, ignore
	if (!response.contains(QStringLiteral("?code=")))
	{
		socket->close();
		return;
	}

	// Do magic with code received
	// GET /?code=<code> HTTP...
	auto left = response.left(response.indexOf(QStringLiteral(" HTTP")));
	auto code = left.right(left.length() - left.indexOf(QStringLiteral("?code=")) - 6);

	auto status = auth.get(code, redirectUrl(),
		QString::fromStdString(settings.account.client_id),
		QString::fromStdString(settings.account.client_secret));

	// Write
	socket->write(QString("HTTP/1.1 200 OK\r\n\r\n%1")
		.arg(status.isEmpty()
			? QString("Success, you can now return to %1").arg(APP_NAME)
			: QString("Failed to authenticate: %1").arg(status))
		.toUtf8());

	socket->flush();
	socket->waitForBytesWritten(writeTimeout);
	socket->close();

	if (!status.isEmpty())
	{
		emit failed(status);
		return;
	}

	settings.save();
	close();
	emit success();
}
