#include "lib/spotifyappstatus.hpp"

#include <QNetworkReply>

SpotifyAppStatus::SpotifyAppStatus(const lib::settings &settings, QObject *parent)
	: QObject(parent),
	http(new QNetworkAccessManager(this)),
	settings(settings)
{
}

void SpotifyAppStatus::version()
{
	const QString accessToken = QString::fromStdString(settings.account.access_token);
	if (accessToken.isEmpty())
	{
		qWarning() << "No access token";
		emit versionFinished(SpotifyAppVersion::Unknown);
		return;
	}

	const QUrl url = QStringLiteral("https://api.spotify.com/v1/albums/6eUW0wxWtzkFdaEFsTJto6");
	QNetworkRequest request(url);
	request.setRawHeader(
		QByteArray::fromStdString("Authorization"),
		QStringLiteral("Bearer %1").arg(accessToken).toUtf8()
	);
	request.setHeader(
		QNetworkRequest::ContentTypeHeader,
		QByteArray::fromStdString("application/json")
	);

	QNetworkReply *reply = http->get(request);
	connect(reply, &QNetworkReply::finished, [this, reply]() -> void
	{
		if (reply->error() != QNetworkReply::NoError)
		{
			qWarning() << reply->errorString();
			emit versionFinished(SpotifyAppVersion::Unknown);
			return;
		}

		QJsonParseError parseError;
		const QJsonDocument json = QJsonDocument::fromJson(reply->readAll(), &parseError);
		reply->deleteLater();

		if (parseError.error != QJsonParseError::NoError)
		{
			qWarning() << parseError.errorString();
			emit versionFinished(SpotifyAppVersion::Unknown);
			return;
		}

		// v3 deprecated "available_markets"
		if (!json.object().contains(QStringLiteral("available_markets")))
		{
			emit versionFinished(SpotifyAppVersion::Version3);
			return;
		}

		const QJsonObject item = json.object()
			.value(QStringLiteral("tracks")).toObject()
			.value(QStringLiteral("items")).toArray()
			.first().toObject();

		// v2 deprecated "preview_url"
		if (!item.contains(QStringLiteral("preview_url")))
		{
			emit versionFinished(SpotifyAppVersion::Version2);
			return;
		}

		emit versionFinished(SpotifyAppVersion::Version1);
	});
}
