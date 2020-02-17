#include "spotify.hpp"

using namespace spt;

Spotify::Spotify()
{
	lastAuth = new QDateTime();
	networkManager = new QNetworkAccessManager();
}

Spotify::~Spotify()
{
	delete lastAuth;
	delete networkManager;
}

QNetworkRequest Spotify::request(QString &url)
{
	// Prepare request
	QNetworkRequest request((QUrl("https://api.spotify.com/v1/" + url)));
	// Set header
	request.setRawHeader("Authorization", ("Bearer " + accessToken()).toUtf8());
	// Return prepared header
	return request;
}

QJsonDocument Spotify::get(QString &url)
{
	// Send request
	auto reply = networkManager->get(request(url));
	// Wait for request to finish
	while (reply->isRunning()) {}
	// Parse reply as json
	QJsonParseError jsonError{};
	auto json = QJsonDocument::fromJson(reply->readAll(), &jsonError);
	if (json.isNull())
		qWarning() << "warning: there was an error parsing json:" << jsonError.errorString();
	// Return parsed json
	return json;
}

QJsonDocument Spotify::put(QString &url, QVariantMap &body)
{
	// Set in header we're sending json data
	auto req = request(url);
	req.setHeader(QNetworkRequest::ContentTypeHeader, QString("application/json"));
	// Send the request, we don't expect any response
	networkManager->put(req, QJsonDocument::fromVariant(body).toJson());
}

QString Spotify::accessToken()
{
	return QSettings().value("AccessToken").toString();
}

QString Spotify::refreshToken()
{
	return QSettings().value("RefreshToken").toString();
}

QString Spotify::clientId()
{
	return QProcessEnvironment().value("SPOTIFY_QT_ID");
}

QString Spotify::clientSecret()
{
	return QProcessEnvironment().value("SPOTIFY_QT_SECRET");
}