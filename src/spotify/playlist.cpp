#include "playlist.hpp"

using namespace spt;

Playlist::Playlist(bool collaborative, QString description, QString id, QString image, QString name,
				   bool isPublic, QJsonObject tracks)
	: collaborative(collaborative), description(description), id(id), image(image),
	  name(name), isPublic(isPublic), tracks(tracks)
{}

QVector<Track> *Playlist::loadTracks(Spotify &spotify)
{
	// Allocate memory for all tracks
	auto trackList = new QVector<Track>(tracks["total"].toInt());
	// Load tracks
	auto href = tracks["href"].toString();
	loadTracksFromUrl(trackList, href, 0, spotify);
	// Return final track list
	return trackList;
}

bool Playlist::loadTracksFromUrl(QVector<Track> *trackList, QString &url, int offset, Spotify &spotify)
{
	// Load tracks from api
	auto newUrl = url.remove(0, QString("https://api.spotify.com/v1/").length());
	auto current = spotify.get(newUrl);
	// Load from url
	auto items = current["items"].toArray();
	for (int i = 0; i < items.size(); i++)
		(*trackList)[offset + i] = Track(items.at(i).toObject());
	// Check if there's a next page
	auto nextPage = current["next"].toString();
	if (!nextPage.isEmpty())
		loadTracksFromUrl(trackList, nextPage, offset + items.size(), spotify);
	return true;
}