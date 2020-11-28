#include "artist.hpp"

using namespace lib::spt;

Artist::Artist(const nlohmann::json &json)
{
	json.at("id").get_to(id);
	json.at("followers").get_to(followers);
	json.at("popularity").get_to(popularity);
	json.at("genres").get_to(genres);
	json.at("name").get_to(name);
	json.at("images").at(1).at("url").get_to(image);
	json.at("external_urls").get_to(externalUrls);
}

std::vector<Track> Artist::topTracks(Spotify &spotify) const
{
	auto json = spotify.getAsObject(QString("artists/%1/top-tracks?country=from_token").arg(id));
	auto items = json["tracks"].toArray();
	QVector<Track> tracks;
	tracks.reserve(items.size());
	for (auto item : items)
		tracks.append(Track(item.toObject()));
	return tracks;
}

std::vector<Album> Artist::albums(Spotify &spotify) const
{
	auto json = spotify.getAsObject(QString("artists/%1/albums?country=from_token").arg(id));
	auto items = json["items"].toArray();
	QVector<Album> albums;
	albums.reserve(items.size());
	for (auto item : items)
		albums.append(Album(item.toObject()));
	return albums;
}

std::vector<Artist> Artist::relatedArtists(Spotify &spotify) const
{
	auto json = spotify.getAsObject(QString("artists/%1/related-artists").arg(id));
	auto items = json["artists"].toArray();
	QVector<Artist> artists;
	artists.reserve(items.size());
	for (auto item : items)
		artists.append(Artist(item.toObject()));
	return artists;
}

nlohmann::json Artist::toJson() const
{
	return {
		{"followers", followers},
		{"popularity", popularity},
		{"genres", genres},
		{"id", id},
		{"name", name},
		{"image", image}
	};
}