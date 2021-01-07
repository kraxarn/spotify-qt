#include "auth.hpp"

using namespace spt;

Auth::Auth(lib::settings &settings)
	: settings(settings)
{
	networkManager = new QNetworkAccessManager();
}

Auth::~Auth()
{
	delete networkManager;
}

QString Auth::authUrl(const QString &clientId, const QString &redirect)
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
	return QString("https://accounts.spotify.com/authorize?client_id=%1&response_type=code&redirect_uri=%2&scope=%3")
		.arg(clientId)
		.arg(QString(redirectUrl.toEncoded()))
		.arg(scopes.join("%20"));
}

QString Auth::auth(const QString &code, const QString &redirect, const QString &id, const QString &secret)
{
	if (code.isEmpty())
		return QString("no code specified");
	QUrl redirectUrl(redirect);

	// Prepare form to send
	auto postData = QString("grant_type=authorization_code&code=%1&redirect_uri=%2")
		.arg(code)
		.arg(QString(redirectUrl.toEncoded()))
		.toUtf8();

	// Prepare request
	QNetworkRequest request(QUrl("https://accounts.spotify.com/api/token"));
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
	request.setRawHeader("Authorization",
		"Basic " + QString("%1:%2").arg(id).arg(secret).toUtf8().toBase64());

	// Send request
	auto reply = networkManager->post(request, postData);

	// Wait for response
	while (!reply->isFinished())
		QCoreApplication::processEvents();
	auto jsonData = QJsonDocument::fromJson(reply->readAll()).object();
	reply->deleteLater();
	if (jsonData.contains("error_description"))
		return jsonData["error_description"].toString();

	// Save access/refresh token to settings
	auto accessToken = jsonData["access_token"].toString().toStdString();
	auto refreshToken = jsonData["refresh_token"].toString().toStdString();
	settings.account.access_token = accessToken;
	settings.account.refresh_token = refreshToken;
	settings.save();

	// Everything hopefully went fine
	return QString();
}