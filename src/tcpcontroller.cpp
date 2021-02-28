#include "tcpcontroller.hpp"

TcpController::TcpController(QObject *parent)
	: QTcpServer(parent)
{
	if (!listen(QHostAddress::LocalHost, 8888))
	{
		lib::log::error("Failed to start TCP listener: {}",
			errorString().toStdString());
	}
}

QTcpSocket *TcpController::nextPendingConnection()
{
	return QTcpServer::nextPendingConnection();
}

void TcpController::incomingConnection(qintptr handle)
{
	QTcpServer::incomingConnection(handle);

	auto client = nextPendingConnection();
	if (client == nullptr)
		return;

	QTcpSocket::connect(client, &QAbstractSocket::disconnected, client, &QObject::deleteLater);
	QTcpSocket::connect(client, &QAbstractSocket::readyRead, [this, client]()
	{
		this->clientData(client);
	});
}

void TcpController::clientData(QTcpSocket *client)
{
	if (client == nullptr)
		return;
	client->write(QString::fromStdString(command(QString(client->readAll())
		.trimmed().toStdString(), client).dump()).toUtf8());
}

nlohmann::json TcpController::command(const std::string &message, QTcpSocket *client)
{
	if (commands.empty())
	{
		commands["version"] = []()
		{
			return nlohmann::json({
				{"name", QCoreApplication::applicationName().toStdString()},
				{"version", QCoreApplication::applicationVersion().toStdString()},
			});
		};

		commands["status"] = [this]()
		{
			auto mainWindow = dynamic_cast<MainWindow *>(this->parent());
			return this->toJson(mainWindow->currentPlayback().toJson());
		};

		commands["play-pause"] = [this]()
		{
			auto mainWindow = dynamic_cast<MainWindow *>(this->parent());
			auto status = mainWindow->playPause();

			return status.isEmpty()
				? nlohmann::json()
				: nlohmann::json({
					{"error", status.toStdString()},
				});
		};

		commands["close"] = [client]()
		{
			client->disconnectFromHost();
			return nlohmann::json();
		};

		commands["help"] = [this]()
		{
			std::vector<std::string> keys;
			for (auto &command : this->commands)
			{
				if (command.first == "help")
					continue;
				keys.push_back(command.first);
			}
			return nlohmann::json({
				{"commands", keys},
			});
		};
	}

	return commands.count(message) > 0
		? commands[message]()
		: nlohmann::json({
			{"error", "no such command"}
		});
}

nlohmann::json TcpController::toJson(const QJsonObject &json)
{
	return nlohmann::json(QString(QJsonDocument(json)
		.toJson(QJsonDocument::Compact))
		.toStdString());
}
