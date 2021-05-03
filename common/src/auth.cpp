#include "common/spotify/auth.hpp"

cmn::spt::Auth::Auth(lib::settings &settings, QObject *parent)
	: settings(settings)
{
	networkManager = new QNetworkAccessManager(parent);
}

auto cmn::spt::Auth::authUrl(const QString &clientId, const QString &redirect) -> QString
{
	// Scopes for request, for clarity
	// For now, these are mostly identical to spotify-tui
	QStringList scopes = {
		"playlist-read-collaborative",
		"playlist-read-private",
		"playlist-modify-private",
		"playlist-modify-public",
		"user-follow-read",
		"user-follow-modify",
		"user-library-modify",
		"user-library-read",
		"user-modify-playback-state",
		"user-read-currently-playing",
		"user-read-playback-state",
		"user-read-private",
		"user-read-recently-played",
		"streaming",
		"user-read-email",
		"user-top-read"
	};

	// Prepare url and open browser
	QUrl redirectUrl(redirect);
	return QString(
		"https://accounts.spotify.com/authorize?client_id=%1&response_type=code&redirect_uri=%2&scope=%3")
		.arg(clientId, QString(redirectUrl.toEncoded()), scopes.join("%20"));
}

auto cmn::spt::Auth::auth(const QString &code, const QString &redirect,
	const QString &id, const QString &secret) -> QString
{
	if (networkManager == nullptr)
	{
		return QString("invalid network");
	}

	if (code.isEmpty())
	{
		return QString("no code specified");
	}
	QUrl redirectUrl(redirect);

	// Prepare form to send
	auto postData = QString("grant_type=authorization_code&code=%1&redirect_uri=%2")
		.arg(code, QString(redirectUrl.toEncoded()))
		.toUtf8();

	// Prepare request
	QNetworkRequest request(QUrl("https://accounts.spotify.com/api/token"));
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
	request.setRawHeader("Authorization",
		"Basic " + QString("%1:%2").arg(id, secret).toUtf8().toBase64());

	// Send request
	auto *reply = networkManager->post(request, postData);

	// Wait for response
	while (!reply->isFinished())
	{
		QCoreApplication::processEvents();
	}
	auto jsonData = QJsonDocument::fromJson(reply->readAll()).object();
	reply->deleteLater();
	if (jsonData.contains("error_description"))
	{
		return jsonData["error_description"].toString();
	}

	// Save access/refresh token
	settings.account.access_token = jsonData["access_token"].toString().toStdString();
	settings.account.refresh_token = jsonData["refresh_token"].toString().toStdString();

	// Everything hopefully went fine
	return QString();
}
