#include "spotifyapi.hpp"

// Currently unavailable:
// artists

void Spotify::artist(const QString &artistId, lib::callback<spt::Artist> &callback)
{
	get(QString("artists/%1").arg(artistId), [callback](const QJsonObject &json)
	{
		callback(Artist(json));
	});
}

void Spotify::topTracks(const spt::Artist &artist,
	lib::callback<std::vector<lib::spt::track>> &callback)
{
	get<lib::spt::track>(lib::fmt::format("artists/{}/top-tracks?country=from_token",
		artist.id.toStdString()), "tracks", callback);
}

void Spotify::relatedArtists(const spt::Artist &artist,
	lib::callback<std::vector<spt::Artist>> &callback)
{
	get<spt::Artist>(QString("artists/%1/related-artists")
		.arg(artist.id), callback);
}

void Spotify::albums(const spt::Artist &artist,
	lib::callback<std::vector<lib::spt::album>> &callback)
{
	get<lib::spt::album>(lib::fmt::format("artists/{}/albums?country=from_token",
		artist.id.toStdString()), "items", callback);
}
