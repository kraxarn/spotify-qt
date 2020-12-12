#include "artist.hpp"

using namespace lib::spt;

artist::artist(const nlohmann::json &json)
{
	json.at("id").get_to(id);
	json.at("followers").get_to(followers);
	json.at("popularity").get_to(popularity);
	json.at("genres").get_to(genres);
	json.at("name").get_to(name);
	json.at("images").at(1).at("url").get_to(image);
	json.at("external_urls").get_to(external_urls);
}

std::vector<track> artist::topTracks(spotify &spotify) const
{
	auto json = spotify.getAsObject(QString("artists/%1/top-tracks?country=from_token").arg(id));
	auto items = json["tracks"].toArray();
	QVector<Track> tracks;
	tracks.reserve(items.size());
	for (auto item : items)
		tracks.append(track(item.toObject()));
	return tracks;
}

std::vector<Album> artist::albums(spotify &spotify) const
{
	auto json = spotify.getAsObject(QString("artists/%1/albums?country=from_token").arg(id));
	auto items = json["items"].toArray();
	QVector<Album> albums;
	albums.reserve(items.size());
	for (auto item : items)
		albums.append(Album(item.toObject()));
	return albums;
}

std::vector<artist> artist::relatedArtists(spotify &spotify) const
{
	auto json = spotify.getAsObject(QString("artists/%1/related-artists").arg(id));
	auto items = json["artists"].toArray();
	QVector<artist> artists;
	artists.reserve(items.size());
	for (auto item : items)
		artists.append(artist(item.toObject()));
	return artists;
}

nlohmann::json artist::to_json() const
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