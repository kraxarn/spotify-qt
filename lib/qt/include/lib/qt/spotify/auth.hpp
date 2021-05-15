#pragma once

#include "lib/settings.hpp"
#include "lib/spotify/auth.hpp"
#include "lib/qt/httpclient.hpp"

#include <QCoreApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QString>
#include <QUrl>

namespace lib
{
	namespace qt
	{
		namespace spt
		{
			class auth: public QObject
			{
			Q_OBJECT

			public:
				explicit auth(lib::settings &settings, QObject *parent);

				/**
				 * Authentication URL
				 */
				static auto auth_url(const QString &client_id, const QString &redirect) -> QString;

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
				auto get(const QString &code, const QString &redirect,
					const QString &id, const QString &secret) -> QString;

			private:
				lib::spt::auth *spt_auth = nullptr;
			};
		}
	}
}
