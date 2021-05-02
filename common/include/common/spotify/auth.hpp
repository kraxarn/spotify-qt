#pragma once

#include <QCoreApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QString>
#include <QUrl>

namespace cmn
{
	namespace spt
	{
		class Auth
		{
		public:
			explicit Auth(QObject *parent);

			/**
			 * Authentication URL
			 */
			static auto authUrl(const QString &clientId, const QString &redirect) -> QString;

			/**
			 * Authenticate
			 * @param code Authorization code
			 * @param redirect Redirect URL
			 * @param id Client ID
			 * @param secret Client Secret
			 * @param accessToken Outputted access token
			 * @param refreshToken Outputted refresh token
			 * @return Error, or empty string on success
			 */
			auto auth(const QString &code, const QString &redirect, const QString &id,
				const QString &secret, QString &accessToken, QString &refreshToken) -> QString;

		private:
			QNetworkAccessManager *networkManager = nullptr;
		};
	}
}
