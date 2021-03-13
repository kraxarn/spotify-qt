#include "spotifyapi.hpp"

// Currently unavailable:
// artists

void Spotify::artist(const QString &artistId, callback<spt::Artist> &callback)
{
	get(QString("artists/%1").arg(artistId), [callback](const QJsonObject &json)
	{
		callback(Artist(json));
	});
}

void Spotify::topTracks(const spt::Artist &artist, callback<std::vector<spt::Track>> &callback)
{
	get<spt::Track>(QString("artists/%1/top-tracks?country=from_token")
		.arg(artist.id), callback);
}

void Spotify::relatedArtists(const spt::Artist &artist,
	callback<std::vector<spt::Artist>> &callback)
{
	get<spt::Artist>(QString("artists/%1/related-artists")
		.arg(artist.id), callback);
}

void Spotify::albums(const spt::Artist &artist, callback<std::vector<spt::Album>> &callback)
{
	get<spt::Album>(QString("artists/%1/albums?country=from_token")
		.arg(artist.id), callback);
}
