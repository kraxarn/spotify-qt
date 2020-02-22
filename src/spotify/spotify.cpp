#include "spotify.hpp"

using namespace spt;

Spotify::Spotify()
{
	lastAuth = 0;
	networkManager = new QNetworkAccessManager();
	refresh();
}

Spotify::~Spotify()
{
	delete networkManager;
}

QNetworkRequest Spotify::request(QString &url)
{
	// See when last refresh was
	auto lastRefresh = QDateTime::currentSecsSinceEpoch() - lastAuth;
	if (lastRefresh >= 3600)
	{
		qDebug() << "access token probably expired, refreshing";
		refresh();
	}
	// Prepare request
	QNetworkRequest request((QUrl("https://api.spotify.com/v1/" + url)));
	// Set header
	request.setRawHeader("Authorization", ("Bearer " + Settings().accessToken()).toUtf8());
	// Return prepared header
	return request;
}

QJsonDocument Spotify::get(QString url)
{
	// Send request
	auto reply = networkManager->get(request(url));
	// Wait for request to finish
	while (!reply->isFinished())
		QCoreApplication::processEvents();
	// Parse reply as json
	auto json = QJsonDocument::fromJson(reply->readAll());
	reply->deleteLater();
	// Return parsed json
	return json;
}

QString Spotify::put(QString url, QVariantMap *body)
{
	// Set in header we're sending json data
	auto req = request(url);
	req.setHeader(QNetworkRequest::ContentTypeHeader, QString("application/json"));
	// Send the request, we don't expect any response
	auto putData = body == nullptr ? nullptr : QJsonDocument::fromVariant(*body).toJson();
	return errorMessage(networkManager->put(req, putData));
}

QString Spotify::post(QString url)
{
	return errorMessage(networkManager->post(request(url), QByteArray()));
}

QString Spotify::errorMessage(QNetworkReply *reply)
{
	while (!reply->isFinished())
		QCoreApplication::processEvents();
	auto replyBody = reply->readAll();
	reply->deleteLater();
	if (replyBody.isEmpty())
		return QString();
	auto json = QJsonDocument::fromJson(replyBody);
	return json["error"].toObject()["message"].toString();
}

bool Spotify::refresh()
{
	// Make sure we have a refresh token
	Settings settings;
	auto refreshToken = settings.refreshToken();
	if (refreshToken.isEmpty())
	{
		qWarning() << "warning: attempt to refresh without refresh token";
		return false;
	}
	// Create form
	auto postData = QString("grant_type=refresh_token&refresh_token=%1")
		.arg(refreshToken)
		.toUtf8();
	// Create request
	QNetworkRequest request(QUrl("https://accounts.spotify.com/api/token"));
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
	request.setRawHeader(
		"Authorization",
		"Basic " + QString("%1:%2")
		.arg(settings.clientId()).arg(settings.clientSecret()).toUtf8().toBase64());
	// Send request
	auto reply = networkManager->post(request, postData);
	while (!reply->isFinished())
		QCoreApplication::processEvents();
	// Parse json
	auto json = QJsonDocument::fromJson(reply->readAll()).object();
	reply->deleteLater();
	// Check if error
	if (json.contains("error_description"))
	{
		qWarning() << "warning: failed to refresh token:" << json["error_description"];
		return false;
	}
	// Save as access token
	lastAuth = QDateTime::currentSecsSinceEpoch();
	auto accessToken = json["access_token"].toString();
	settings.setAccessToken(accessToken);
	return true;
}

void Spotify::playlists(QVector<Playlist> **playlists)
{
	// Request playlists
	auto json = get("me/playlists?limit=50");
	// Parse as playlists
	auto items = json["items"].toArray();
	// Create list of playlists
	auto size = json["total"].toInt();
	(*playlists)->clear();
	(*playlists)->reserve(size);
	// Loop through all items
	for (int i = 0; i < items.size(); i++)
	{
		auto data = items.at(i).toObject();
		(**playlists).insert(i, Playlist(
			data["collaborative"].toBool(),
			data["description"].toString(),
			data["id"].toString(),
			data["images"].toArray()[0].toObject()["url"].toString(),
			data["name"].toString(),
			data["public"].toBool(),
			data["tracks"].toObject()
		));
	}
}

QVector<Device> Spotify::devices()
{
	auto json = get("me/player/devices");
	auto items = json["devices"].toArray();
	QVector<Device> devices(items.size());
	for (int i = 0; i < items.size(); i++)
	{
		auto data = items.at(i).toObject();
		Device device;
		device.id				= data["id"].toString();
		device.name				= data["name"].toString();
		device.type				= data["type"].toString();
		device.isActive			= data["is_active"].toBool();
		device.isPrivateSession	= data["is_private_session"].toBool();
		device.isRestricted		= data["is_restricted"].toBool();
		device.volumePercent	= data["volume_percent"].toInt();
		devices[i] = device;
	}
	return devices;
}

QString Spotify::setDevice(Device device)
{
	QVariantMap body;
	body["device_ids"] = QStringList({
		device.id
	});
	currentDevice = device.id;
	return put("me/player", &body);
}

QString Spotify::playTracks(const QString &track, const QString &context)
{
	QVariantMap body;
	body["context_uri"] = context;
	body["offset"] = QJsonObject({
		QPair<QString, QJsonValue>("uri", track)
	});
	return put(currentDevice == nullptr
		? QString("me/player/play")
		: QString("me/player/play?device_id=%1").arg(currentDevice), &body);
}

QString Spotify::setShuffle(bool enabled)
{
	return put(QString("me/player/shuffle?state=%1").arg(enabled ? "true" : "false"));
}

Playback Spotify::currentPlayback()
{
	auto json = get("me/player");
	Playback playback;
	if (json["item"].isNull())
	{
		// No track playing
		playback.progressMs	= 0u;
		playback.item 		= new Track();
		playback.isPlaying 	= false;
		playback.volume 	= 100;
		playback.repeat 	= "off";
		playback.shuffle	= false;
		return playback;
	}
	playback.progressMs	= json["progress_ms"].toInt();
	playback.item 		= new Track(json["item"].toObject());
	playback.isPlaying 	= json["is_playing"].toBool();
	playback.volume 	= json["device"].toObject()["volume_percent"].toInt();
	playback.repeat		= json["repeat_state"].toString();
	playback.shuffle	= json["shuffle_state"].toBool();
	return playback;
}

QString Spotify::pause()
{
	return put("me/player/pause");
}

QString Spotify::resume()
{
	return put("me/player/play");
}

QString Spotify::seek(int position)
{
	return put(QString("me/player/seek?position_ms=%1").arg(position));
}

QString Spotify::next()
{
	return post("me/player/next");
}

QString Spotify::previous()
{
	return post("me/player/previous");
}

QString Spotify::setVolume(int volume)
{
	return put(QString("me/player/volume?volume_percent=%1").arg(volume));
}

QString Spotify::setRepeat(QString state)
{
	return put(QString("me/player/repeat?state=%1").arg(state));
}

AudioFeatures Spotify::trackAudioFeatures(QString trackId)
{
	auto json = get(QString("audio-features/%1")
		.arg(trackId.startsWith("spotify:track:")
			? trackId.remove(0, QString("spotify:track:").length())
			: trackId));
	return AudioFeatures(json.object());
}