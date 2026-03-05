#include "lib/spotify/api.hpp"
#include "lib/spotify/playlistsnapshot.hpp"

void lib::spt::api::create_playlist(const std::string &name,
	const std::optional<std::string> &description,
	const std::optional<bool> &is_public,
	const std::optional<bool> &is_collaborative,
	ApiCallback<lib::spt::playlist> &callback)
{
	nlohmann::json json{
		{"name", name},
	};

	lib::json::set(json, "description", description);
	lib::json::set(json, "public", is_public);
	lib::json::set(json, "collaborative", is_collaborative);

	post("me/playlists", json, callback);
}

void lib::spt::api::playlists(const ApiPagedCallback<spt::playlist> &callback) const
{
	request.get_page<spt::playlist>("me/playlists?limit=50", {}, callback);
}

void lib::spt::api::playlist(const std::string &playlist_id, ApiCallback<Result<spt::playlist>> &callback) const
{
	request.get(fmt::format("playlists/{}", playlist_id), callback);
}

void lib::spt::api::edit_playlist(const std::string &playlist_id,
	const lib::spt::playlist_details &playlist,
	ApiCallback<std::string> &callback)
{
	put(lib::fmt::format("playlists/{}", playlist_id), playlist, callback);
}

void lib::spt::api::playlist_tracks(const spt::playlist &playlist,
	const std::function<bool(const Result<page<spt::track>> &)> &callback) const
{
	const char *endpoint;
	switch (playlist.version)
	{
		case PlaylistVersion::Version1:
			endpoint = "tracks";
			break;

		case PlaylistVersion::Version2:
			endpoint = "items";
			break;

		default:
			callback(Result<page<spt::track>>::fail(QStringLiteral("Unknown playlist version")));
			return;
	}

	const std::string url = fmt::format("playlists/{}/{}?market=from_token&limit=50",
		playlist.id, endpoint);

	request.get_page<lib::spt::track>(url, std::string(), callback);
}

void lib::spt::api::add_to_playlist(const spt::playlist &playlist,
	const std::vector<std::string> &track_uris,
	ApiCallback<Result<PlaylistSnapshot>> &callback) const
{
	const QString path = QStringLiteral("playlists/%1/tracks")
		.arg(QString::fromStdString(playlist.id));

	QJsonArray tracks;
	for (const std::string &uri: track_uris)
	{
		tracks.append(QString::fromStdString(uri));
	}

	QJsonObject body;
	body[QStringLiteral("uris")] = tracks;

	request.post(path, QJsonDocument(body), callback);
}

void lib::spt::api::remove_from_playlist(const spt::playlist &playlist,
	const std::vector<std::string> &track_uris,
	ApiCallback<Result<PlaylistSnapshot>> &callback) const
{
	const QString path = QStringLiteral("playlists/%1/tracks")
		.arg(QString::fromStdString(playlist.id));

	QJsonArray tracks;
	for (const std::string &uri: track_uris)
	{
		QJsonObject track;
		track[QStringLiteral("uri")] = QString::fromStdString(uri);
		tracks.append(track);
	}

	QJsonObject body;
	body[QStringLiteral("tracks")] = tracks;
	body[QStringLiteral("snapshot_id")] = QString::fromStdString(playlist.snapshot);

	request.deleteResource(path, QJsonDocument(body), callback);
}
