#include "spotifyqml.hpp"

SpotifyQml::SpotifyQml(QObject *parent)
	: QObject(parent)
{
	spotify = new spt::Spotify(settings);
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
