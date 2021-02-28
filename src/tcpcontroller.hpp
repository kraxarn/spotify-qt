#pragma once

#include <QTcpServer>
#include <QTcpSocket>
#include <QCoreApplication>

#include <map>
#include <functional>

#include "lib/log.hpp"
#include "lib/strings.hpp"
#include "thirdparty/json.hpp"

#include "mainwindow.hpp"

class TcpController: public QTcpServer
{
public:
	explicit TcpController(QObject *parent);

	QTcpSocket *nextPendingConnection() override;

protected:
	void incomingConnection(qintptr handle) override;

private:
	void clientData(QTcpSocket *client);
	nlohmann::json command(const std::string &message, QTcpSocket *client);

	std::map<std::string, std::function<nlohmann::json()>> commands;

	nlohmann::json toJson(const QJsonObject &json);
};
