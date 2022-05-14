#include "server.hpp"
#include "lib/log.hpp"
#include "commandline/args.hpp"
#include "mainwindow.hpp"

#define RESPONSE_OK QStringLiteral("ok")

Ipc::Server::Server(lib::spt::api &spotify, QObject *parent)
	: QLocalServer(parent),
	spotify(spotify)
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
	QByteArray data;
	stream >> data;

	if (!stream.atEnd())
	{
		return;
	}

	QByteArray buffer;
	QDataStream out(&buffer, QIODevice::WriteOnly);

	out << onReadAll(data);

	socket->write(buffer);
	socket->flush();
	socket->disconnectFromServer();
}

void Ipc::Server::logStatus(const QString &data, const std::string &status)
{
	if (status.empty())
	{
		return;
	}

	lib::log::warn("{} failed: {}", data.toStdString(), status);
}

auto Ipc::Server::onReadAll(const QString &data) -> QString
{
	auto *mainWindow = qobject_cast<MainWindow *>(parent());
	if (mainWindow == nullptr)
	{
		return QString("%1 failed: no window").arg(data);
	}

	if (data == ARG_PLAY_PAUSE)
	{
		auto callback = [data](const std::string &status)
		{
			Ipc::Server::logStatus(data, status);
		};

		if (mainWindow->currentPlayback().is_playing)
		{
			spotify.pause(callback);
		}
		else
		{
			spotify.resume(callback);
		}

		return RESPONSE_OK;
	}

	if (data == ARG_PREVIOUS_TRACK)
	{
		spotify.previous([data](const std::string &status)
		{
			Ipc::Server::logStatus(data, status);
		});
		return RESPONSE_OK;
	}

	if (data == ARG_NEXT_TRACK)
	{
		spotify.next([data](const std::string &status)
		{
			Ipc::Server::logStatus(data, status);
		});
		return RESPONSE_OK;
	}

	if (data == ARG_METADATA)
	{
		nlohmann::json playback = mainWindow->getCurrentPlayback();
		return QString::fromStdString(playback.dump());
	}

	return QString("Unrecognized command: %1").arg(data);
}
