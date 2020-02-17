#pragma once

#include <QDateTime>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QSettings>
#include <QProcessEnvironment>
#include <QJsonDocument>
#include <QString>

namespace spt
{
	class Spotify : QObject
	{
	public:
		Spotify();
		~Spotify() override;
	private:
		QDateTime *lastAuth;
		QNetworkAccessManager *networkManager;
		/**
		 * Prepare network request with auth header
		 */
		QNetworkRequest request(QString &url);
		/**
		 * HTTP GET request expecting JSON response
		 */
		QJsonDocument get(QString &url);
		/**
		 * HTTP PUT request expecting JSON response
		 */
		QJsonDocument put(QString &url, QVariantMap &body);

		static QString accessToken();
		static QString refreshToken();
		static QString clientId();
		static QString clientSecret();
	};
}