#include "spotifyqml.hpp"

SpotifyQml::SpotifyQml(QObject *parent)
	: QObject(parent)
{
	spotify = new spt::Spotify(settings);

	spt::Spotify::connect(spotify, &spt::Spotify::gotPlayback, [this](const spt::Playback &playback)
	{
		refreshed(playback);
	});

	auto timer = new QTimer(this);
	QTimer::connect(timer, &QTimer::timeout, this, &SpotifyQml::refresh);
	refresh();
	timer->start(1000);
}

void SpotifyQml::refresh()
{
	// TODO: For now this is a copy from MainWindow.cpp

	if (refreshCount < 0
		|| ++refreshCount >= settings.general.refreshInterval
		|| current.progressMs + 1000 > current.item.duration)
	{
		spotify->requestCurrentPlayback();
		refreshCount = 0;
		return;
	}
	// Assume last refresh was 1 sec ago
	if (!current.isPlaying)
		return;
	current.progressMs += 1000;
	refreshed(current);
}

void SpotifyQml::refreshed(const spt::Playback &playback)
{
	// Update the actual UI and stuff from QML
	current = playback;
	emit playbackChanged();
}

QJsonArray SpotifyQml::getPlaylists()
{
	QJsonArray items;
	for (auto &playlist : spotify->playlists())
		items.append(playlist.toJson());
	return items;
}

SpotifyQml::~SpotifyQml()
{
	delete spotify;
}

QJsonObject SpotifyQml::getPlayback()
{
	return current.toJson();
}

QJsonArray SpotifyQml::tracksToJson(QVector<spt::Track> &tracks)
{
	std::sort(tracks.begin(), tracks.end(), [](const spt::Track &t1, const spt::Track &t2) {
		return t1.artist < t2.artist;
	});

	QJsonArray items;
	for (auto &track : tracks)
		items.append(track.toJson());
	return items;
}

QJsonArray SpotifyQml::getPlaylistTracks(const QString &playlistId)
{
	auto tracks = spotify->playlist(playlistId).loadTracks(*spotify);
	return tracksToJson(tracks);
}

QJsonArray SpotifyQml::getAlbumTracks(const QString &albumId)
{
	auto tracks = spotify->albumTracks(albumId);
	return tracksToJson(tracks);
}

QString SpotifyQml::playTracksWithContext(int trackIndex, const QString &context)
{
	return spotify->playTracks(trackIndex, context);
}

QString SpotifyQml::playTrack(const QString &track)
{
	return spotify->playTracks(0, QStringList(track));
}

void SpotifyQml::requestPlayback()
{
	spotify->requestCurrentPlayback();
	refreshCount = 0;
}

QString SpotifyQml::systemInfo()
{
	return SystemInfoDialog::systemInfo(current);
}

QJsonObject SpotifyQml::search(const QString &query)
{
	return spotify->search(query).toJson();
}

QJsonObject SpotifyQml::getArtist(const QString &id)
{
	return spotify->artist(id).toJson();
}

QJsonObject SpotifyQml::getArtistPages(const QString &id)
{
	auto artist = spotify->artist(id);

	QJsonArray popular;
	for (auto &track : artist.topTracks(*spotify))
		popular.append(track.toJson());

	QJsonArray albums, singles;
	for (auto &album : artist.albums(*spotify))
	{
		if (album.albumGroup == "single")
			singles.append(album.toJson());
		else
			albums.append(album.toJson());
	}

	QJsonArray related;
	for (auto &item : artist.relatedArtists(*spotify))
		related.append(item.toJson());

	return QJsonObject(
		{
			QPair<QString, QJsonArray>("popular", popular),
			QPair<QString, QJsonArray>("albums", albums),
			QPair<QString, QJsonArray>("singles", singles),
			QPair<QString, QJsonArray>("related", related)
		}
	);
}