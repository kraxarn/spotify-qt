#include "spotifyapi.hpp"

// Currently unavailable:
// artists

// Deprecated
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

void Spotify::topTracks(const spt::Artist &artist,
	const std::function<void(const std::vector<spt::Track> &tracks)> &callback)
{
	get<spt::Track>(QString("artists/%1/top-tracks?country=from_token").arg(artist.id), callback);
}

void Spotify::relatedArtists(const spt::Artist &artist,
	const std::function<void(const std::vector<spt::Artist> &artists)> &callback)
{
	get<spt::Artist>(QString("artists/%1/related-artists").arg(artist.id), callback);
}

void Spotify::albums(const spt::Artist &artist,
	const std::function<void(const std::vector<spt::Album> &albums)> &callback)
{
	get<spt::Album>(QString("artists/%1/albums?country=from_token").arg(artist.id), callback);
}
