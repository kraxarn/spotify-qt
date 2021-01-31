#include "spotify.hpp"

using namespace spt;

Spotify::Spotify(lib::settings &settings, QObject *parent)
	: settings(settings),
	QObject(parent)
{
	lastAuth = 0;
	networkManager = new QNetworkAccessManager(this);

	if (secondsSinceEpoch() - settings.account.last_refresh < 3600)
	{
		lib::log::info("Last refresh was less than an hour ago, not refreshing access token");
		lastAuth = settings.account.last_refresh;
		refreshValid = true;
		return;
	}

	refreshValid = refresh();
}

QNetworkRequest Spotify::request(const QString &url)
{
	// See when last refresh was
	auto lastRefresh = secondsSinceEpoch() - lastAuth;
	if (lastRefresh >= 3600)
	{
		lib::log::info("Access token probably expired, refreshing");
		refresh();
	}

	// Prepare request
	QNetworkRequest request((QUrl("https://api.spotify.com/v1/" + url)));

	// Set header
	request.setRawHeader("Authorization",
		QString("Bearer %1")
			.arg(QString::fromStdString(settings.account.access_token))
			.toUtf8());

	// Return prepared header
	return request;
}

QJsonDocument Spotify::get(const QString &url)
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
	errorMessage(json, QUrl(url));
	return json;
}

QJsonObject Spotify::getAsObject(const QString &url)
{
	return get(url).object();
}

QJsonArray Spotify::getAsArray(const QString &url)
{
	return get(url).array();
}

void Spotify::get(const QString &url,
	const std::function<void(const QJsonDocument &json)> &callback)
{
	// Prepare fetch of request
	auto context = new QObject();
	QNetworkAccessManager::connect(networkManager, &QNetworkAccessManager::finished, context,
		[context, url, callback](QNetworkReply *reply)
		{
			auto replyUrl = reply->url().toString();
			if (replyUrl.right(replyUrl.length() - 27) != url)
				return;
			delete context;
			// Parse reply as json
			auto json = QJsonDocument::fromJson(reply->readAll());
			reply->deleteLater();
			callback(json);
		});

	networkManager->get(request(url));
}

QString Spotify::put(const QString &url, QVariantMap *body)
{
	// Set in header we're sending json data
	auto req = request(url);
	req.setHeader(QNetworkRequest::ContentTypeHeader, QString("application/json"));

	// Send the request, we don't expect any response
	auto putData = body == nullptr ? nullptr : QJsonDocument::fromVariant(*body).toJson();
	auto reply = errorMessage(networkManager->put(req, putData));
	if (reply.contains("No active device found"))
	{
		auto d = devices();
		if (d.length() == 1)
		{
			setDevice(devices().at(0));
			return put(url, body);
		}
		else if (d.length() > 1)
		{
			DeviceSelectDialog dialog(d);
			if (dialog.exec() == QDialog::Accepted)
			{
				auto selected = dialog.selectedDevice();
				if (!selected.id.isEmpty())
				{
					setDevice(selected);
					return put(url, body);
				}
			}
		}
	}
	return reply;
}

QString Spotify::post(const QString &url)
{
	auto req = request(url);
	req.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
	return errorMessage(networkManager->post(req, QByteArray()));
}

QString Spotify::del(const QString &url, const QJsonDocument &json)
{
	auto req = request(url);
	req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
	return errorMessage(networkManager->sendCustomRequest(req, "DELETE", json.toJson()));
}

QString Spotify::errorMessage(QNetworkReply *reply)
{
	while (!reply->isFinished())
		QCoreApplication::processEvents();
	auto replyBody = reply->readAll();
	reply->deleteLater();
	if (replyBody.isEmpty())
		return QString();

	return errorMessage(QJsonDocument::fromJson(replyBody), reply->url());
}

QString Spotify::errorMessage(const QJsonDocument &json, const QUrl &url)
{
	if (!json.isObject() || !json.object().contains("error"))
		return QString();

	auto message = json.object()["error"].toObject()["message"].toString();
	if (!message.isEmpty())
		lib::log::error("{} failed: {}", url.path().toStdString(), message.toStdString());
	return message;
}

bool Spotify::refresh()
{
	// Make sure we have a refresh token
	auto refreshToken = settings.account.refresh_token;
	if (refreshToken.empty())
	{
		lib::log::warn("Attempt to refresh without refresh token");
		return false;
	}

	// Create form
	auto postData = QString("grant_type=refresh_token&refresh_token=%1")
		.arg(QString::fromStdString(refreshToken))
		.toUtf8();

	// Create request
	QNetworkRequest request(QUrl("https://accounts.spotify.com/api/token"));
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
	request.setRawHeader("Authorization", "Basic " + QString("%1:%2")
		.arg(QString::fromStdString(settings.account.client_id))
		.arg(QString::fromStdString(settings.account.client_secret))
		.toUtf8().toBase64());

	// Send request
	auto reply = networkManager->post(request, postData);
	while (!reply->isFinished())
		QCoreApplication::processEvents();

	// Parse json
	auto json = QJsonDocument::fromJson(reply->readAll()).object();
	reply->deleteLater();

	// Check if error
	if (json.contains("error_description") || !json.contains("access_token"))
	{
		auto error = json["error_description"].toString();
		lib::log::warn("Failed to refresh token: {}", error.isEmpty()
			? "no access token"
			: error.toStdString());
		return false;
	}

	// Save as access token
	lastAuth = secondsSinceEpoch();
	auto accessToken = json["access_token"].toString().toStdString();
	settings.account.last_refresh = lastAuth;
	settings.account.access_token = accessToken;
	settings.save();
	return true;
}

QVector<Playlist> Spotify::playlists(int offset)
{
	// Request playlists
	auto json = getAsObject(QString("me/playlists?limit=50&offset=%1").arg(offset));

	// Parse as playlists
	auto items = json["items"].toArray();

	// Create list of playlists
	auto size = json["total"].toInt();
	QVector<Playlist> playlists;
	playlists.reserve(size);

	// Loop through all items
	for (int i = 0; i < items.size(); i++)
		playlists.insert(i, Playlist(items.at(i).toObject()));

	// Paging
	if (json.contains("next") && !json["next"].isNull())
		playlists.append(this->playlists(json["offset"].toInt() + json["limit"].toInt()));

	return playlists;
}

QVector<Device> Spotify::devices()
{
	auto json = getAsObject("me/player/devices");
	auto items = json["devices"].toArray();
	QVector<Device> devices(items.size());
	for (int i = 0; i < items.size(); i++)
		devices[i] = Device(items.at(i).toObject());
	return devices;
}

QString Spotify::setDevice(const Device &device)
{
	QVariantMap body;
	body["device_ids"] = QStringList({
		device.id
	});
	currentDevice = device.id;
	return put("me/player", &body);
}

QString Spotify::playTracks(int trackIndex, const QString &context)
{
	lib::log::dev("Playing track {} from {}", trackIndex, context.toStdString());

	QVariantMap body;
	body["context_uri"] = context;
	body["offset"] = QJsonObject({
		QPair<QString, int>("position", trackIndex)
	});
	return put(currentDevice == nullptr || currentDevice.isEmpty()
		? QString("me/player/play")
		: QString("me/player/play?device_id=%1").arg(currentDevice), &body);
}

QString Spotify::playTracks(int trackIndex, const QList<QString> &all)
{
	lib::log::dev("Playing track {} ({} total)", trackIndex, all.length());

	auto maxQueue = settings.spotify.max_queue;
	QStringList items = all;
	if (all.length() > maxQueue)
	{
		lib::log::warn("Attempting to queue {} tracks, but only {} allowed",
			all.length(), maxQueue);
		items = all.mid(trackIndex, maxQueue);
		trackIndex = 0;
	}

	QVariantMap body;
	body["uris"] = items;
	body["offset"] = QJsonObject({
		QPair<QString, int>("position", trackIndex)
	});
	return put(currentDevice == nullptr || currentDevice.isEmpty()
		? QString("me/player/play")
		: QString("me/player/play?device_id=%1").arg(currentDevice), &body);
}

QString Spotify::playTracks(const QString &context)
{
	lib::log::dev("Playing track from {}", context.toStdString());

	QVariantMap body;
	body["context_uri"] = context;
	return put(currentDevice == nullptr
		? QString("me/player/play")
		: QString("me/player/play?device_id=%1").arg(currentDevice), &body);
}

QString Spotify::setShuffle(bool enabled)
{
	return put(QString("me/player/shuffle?state=%1").arg(enabled ? "true" : "false"));
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

QString Spotify::setRepeat(const QString &state)
{
	return put(QString("me/player/repeat?state=%1").arg(state));
}

AudioFeatures Spotify::trackAudioFeatures(QString trackId)
{
	auto json = getAsObject(QString("audio-features/%1")
		.arg(trackId.startsWith("spotify:track:")
			? trackId.remove(0, QString("spotify:track:").length())
			: trackId));
	return AudioFeatures(json);
}

QVector<Track> Spotify::albumTracks(const QString &albumId, const QString &albumName, int offset)
{
	auto json = getAsObject(QString("albums/%1/tracks?limit=50&offset=%2")
		.arg(albumId).arg(offset));
	auto trackItems = json["items"].toArray();
	QVector<Track> tracks;
	tracks.reserve(50);

	// Add all in current page
	for (auto item : trackItems)
	{
		auto t = Track(item.toObject());
		// Album name is not included, so we have to set it manually
		t.album = albumName;
		tracks.append(t);
	}

	// Add all in next page
	if (json.contains("next") && !json["next"].isNull())
		tracks.append(albumTracks(albumId, albumName,
			json["offset"].toInt() + json["limit"].toInt()));
	return tracks;
}

QVector<Track> Spotify::albumTracks(const QString &albumID)
{
	auto json = getAsObject(QString("albums/%1").arg(albumID));
	auto albumName = json["name"].toString();
	QVector<Track> tracks;
	tracks.reserve(json["total_tracks"].toInt());

	// Loop through all items
	for (auto track : json["tracks"].toObject()["items"].toArray())
	{
		auto t = Track(track.toObject());
		// Album name is not included, so we have to set it manually
		t.album = albumName;
		tracks.append(t);
	}

	// Check if we have any more to add
	auto tracksLimit = json["tracks"].toObject()["limit"].toInt();
	if (json["tracks"].toObject()["total"].toInt() > tracksLimit)
		tracks.append(albumTracks(albumID, albumName, tracksLimit));

	// Return final vector
	return tracks;
}

Artist Spotify::artist(const QString &artistId)
{
	return Artist(getAsObject(QString("artists/%1").arg(artistId)));
}

void Spotify::artist(const QString &artistId,
	const std::function<void(const spt::Artist &artist)> &callback)
{
	get(QString("artists/%1").arg(artistId), [callback](const QJsonDocument &json)
	{
		callback(Artist(json.object()));
	});
}

Playlist Spotify::playlist(const QString &playlistId)
{
	return Playlist(getAsObject(QString("playlists/%1").arg(playlistId)));
}

QString Spotify::addToPlaylist(const QString &playlistId, const QString &trackId)
{
	return post(QString("playlists/%1/tracks?uris=%2")
		.arg(playlistId).arg(trackId));
}

QString Spotify::removeFromPlaylist(const QString &playlistId, const QString &trackId, int pos)
{
	QJsonDocument json({
		QPair<QString, QJsonArray>("tracks", QJsonArray({
			QJsonObject({
				QPair<QString, QString>("uri", trackId),
				QPair<QString, QJsonArray>("positions", QJsonArray({
					pos
				}))
			})
		}))
	});

	return del(QString("playlists/%1/tracks").arg(playlistId), json);
}

SearchResults Spotify::search(const QString &query)
{
	return SearchResults(getAsObject(
		QString("search?q=%1&type=album,artist,playlist,track&limit=50&market=from_token")
			.arg(query)));
}

template<class T>
QVector<T> Spotify::loadItems(const QString &url)
{
	auto items = getAsObject(url)["items"].toArray();
	QVector<T> result;
	result.reserve(items.count());
	for (auto item : items)
		result.append(T(item.toObject()));
	return result;
}

QVector<Artist> Spotify::topArtists()
{
	return loadItems<Artist>("me/top/artists?limit=10");
}

QVector<Track> Spotify::topTracks()
{
	return loadItems<Track>("me/top/tracks?limit=50");
}

QVector<Track> Spotify::recentlyPlayed()
{
	return loadItems<Track>("me/player/recently-played?limit=50");
}

QVector<Album> Spotify::savedAlbums()
{
	auto json = getAsObject("me/albums");
	auto albumItems = json["items"].toArray();
	QVector<Album> albums;
	albums.reserve(10);
	for (auto item : albumItems)
		albums.append(Album(item.toObject()["album"].toObject()));
	return albums;
}

QVector<Track> Spotify::savedTracks(int offset)
{
	auto json = getAsObject(QString("me/tracks?limit=50&offset=%1").arg(offset));
	auto trackItems = json["items"].toArray();
	QVector<Track> tracks;
	tracks.reserve(50);

	// Add all in current page
	for (auto item : trackItems)
		tracks.append(Track(item.toObject()));

	// Add all in next page
	if (json.contains("next") && !json["next"].isNull())
		tracks.append(savedTracks(json["offset"].toInt() + json["limit"].toInt()));
	return tracks;
}

QString Spotify::addSavedTrack(const QString &trackId)
{
	QVariantMap body;
	body["ids"] = QStringList({
		trackId.startsWith("spotify:track")
			? trackId.right(trackId.length() - QString("spotify:track:").length())
			: trackId
	});
	return put("me/tracks", &body);
}

QString Spotify::removeSavedTrack(const QString &trackId)
{
	QJsonDocument json({
		QPair<QString, QJsonArray>("ids", QJsonArray({
			trackId.startsWith("spotify:track")
				? trackId.right(trackId.length() - QString("spotify:track:").length())
				: trackId
		}))
	});

	return del("me/tracks", json);
}

QVector<Album> Spotify::newReleases(int offset)
{
	auto json = getAsObject(QString("browse/new-releases?limit=50&offset=%1").arg(offset))["albums"]
		.toObject();
	auto albumItems = json["items"].toArray();
	QVector<Album> albums;
	albums.reserve(albumItems.size());
	for (auto item : albumItems)
		albums.append(Album(item.toObject()));
	if (json.contains("next") && !json["next"].isNull())
		albums << newReleases(json["offset"].toInt() + json["limit"].toInt());
	return albums;
}

void Spotify::currentPlayback(const std::function<void(const spt::Playback &playback)> &callback)
{
	get("me/player", [callback](const QJsonDocument &json)
	{
		callback(Playback(json.object()));
	});
}

User Spotify::me()
{
	auto json = getAsObject("me");
	User user;
	user.displayName = json["display_name"].toString();
	user.id = json["id"].toString();
	user.image = json["images"].toArray()[2].toObject()["url"].toString();
	user.product = json["product"].toString();
	return user;
}

QString Spotify::editPlaylist(const Playlist &playlist)
{
	QVariantMap body({
		{"name", playlist.name},
		{"public", playlist.isPublic},
		{"collaborative", playlist.collaborative},
		{"description", playlist.description}
	});
	return put(QString("playlists/%1").arg(playlist.id), &body);
}

QString Spotify::addToQueue(const QString &uri)
{
	return post(QString("me/player/queue?uri=%1").arg(uri));
}

QVector<Artist> Spotify::followedArtists(const QString &offset)
{
	auto json = getAsObject(QString("me/following?type=artist&limit=50%1")
		.arg(offset.isEmpty() ? "" : QString("&after=%1").arg(offset)))["artists"].toObject();
	auto items = json["items"].toArray();
	QVector<Artist> artists;
	artists.reserve(items.size());
	for (auto item : items)
		artists.append(Artist(item.toObject()));
	if (!json["cursors"].isNull() && !json["cursors"].toObject()["after"].isNull())
		artists << followedArtists(json["cursors"].toObject()["after"].toString());
	return artists;
}

void Spotify::isFollowing(FollowType type,
	const QList<QString> &ids,
	const std::function<void(const std::vector<bool> &)> &callback)
{
	get(QString("me/following/contains?type=%1&ids=%2")
		.arg(followTypeString(type))
		.arg(ids.join(',')), [callback](const QJsonDocument &json)
	{
		std::vector<bool> values;
		for (auto value : json.array())
			values.push_back(value.toBool());
		callback(values);
	});
}

void Spotify::topTracks(const spt::Artist &artist,
	const std::function<void(const std::vector<spt::Track> &tracks)> &callback)
{
	get<spt::Track>(QString("artists/%1/top-tracks?country=from_token").arg(artist.id), callback);
}

void Spotify::albums(const spt::Artist &artist,
	const std::function<void(const std::vector<spt::Album> &albums)> &callback)
{
	get<spt::Album>(QString("artists/%1/albums?country=from_token").arg(artist.id), callback);
}

void Spotify::relatedArtists(const spt::Artist &artist,
	const std::function<void(const std::vector<spt::Artist> &artists)> &callback)
{
	get<spt::Artist>(QString("artists/%1/related-artists").arg(artist.id), callback);
}

QString Spotify::followTypeString(FollowType type)
{
	switch (type)
	{
		case FollowType::Artist:
			return "artist";

		case FollowType::User:
			return "user";
	}

	return QString();
}

void Spotify::follow(FollowType type, const QList<QString> &ids)
{
	put(QString("me/following?type=%1&ids=%2")
		.arg(followTypeString(type)).arg(ids.join(',')));
}

void Spotify::unfollow(FollowType type, const QList<QString> &ids)
{
	del(QString("me/following?type=%1&ids=%2")
		.arg(followTypeString(type)).arg(ids.join(',')), {});
}

bool Spotify::isValid() const
{
	return refreshValid;
}

spt::Track Spotify::getTrack(const QString &id)
{
	return spt::Track(getAsObject(QString("tracks/%1").arg(id)));
}

spt::Album Spotify::getAlbum(const QString &id)
{
	return spt::Album(getAsObject(QString("albums/%1").arg(id)));
}

long Spotify::secondsSinceEpoch()
{
	return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now()
		.time_since_epoch()).count();
}
