#include "api.hpp"

using namespace lib::spt;

std::vector<track> api::api::top_tracks(const artist &artist)
{
	auto json = get(fmt::format("artists/{}/top-tracks?country=from_token", artist.id));
	std::vector<track> tracks;
	for (auto &item : json.at("tracks").items())
		tracks.emplace_back(item);
	return tracks;
}

std::vector<Album> api::api::albums(const artist &artist)
{
	auto json = get(fmt::format("artists/{}/albums?country=from_token", artist.id));
	std::vector<Album> albums;
	for (auto &item : json.at("items").items())
		albums.emplace_back(item);
	return albums;
}

std::vector<artist> api::api::related_artists(const artist &artist)
{
	auto json = get(fmt::format("artists/{}/related-artists", artist.id));
	std::vector<spt::artist> artists;
	for (auto &item : json.at("artists").items())
		artists.emplace_back(item);
	return artists;
}