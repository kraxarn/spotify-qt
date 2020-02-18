#pragma once

#include "src/settings.hpp"

#include <QDateTime>
#include <QtNetwork>
#include <QSettings>
#include <QProcessEnvironment>
#include <QJsonDocument>
#include <QJsonObject>
#include <QString>
#include <QDesktopServices>
#include <QInputDialog>
#include <QCoreApplication>

namespace spt
{
	class Spotify : QObject
	{
	public:
		Spotify();
		~Spotify() override;
		/**
		 * Authenticate with Spotify
		 */
		bool auth();
		/**
		 * HTTP GET request expecting JSON response
		 */
		QJsonDocument get(QString &url);
	private:
		QDateTime *lastAuth;
		QNetworkAccessManager *networkManager;
		/**
		 * Prepare network request with auth header
		 */
		QNetworkRequest request(QString &url);
		/**
		 * HTTP PUT request expecting JSON response
		 */
		void put(QString &url, QVariantMap &body);

		bool refresh();

		static QString clientId();
		static QString clientSecret();
	};
}