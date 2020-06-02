#pragma once

#include "src/settings.hpp"

#include <QString>
#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QCoreApplication>
#include <QJsonDocument>
#include <QJsonObject>

namespace spt
{
	class Auth
	{
	public:
		explicit Auth(Settings &settings);
		virtual ~Auth();
		static QString authUrl(const QString &clientId, const QString &redirect);
		QString auth(const QString &code, const QString &redirect, const QString &id, const QString &secret);
	private:
		QNetworkAccessManager *networkManager;
		Settings &settings;
	};
}