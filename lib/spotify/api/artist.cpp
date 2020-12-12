#include "api.hpp"

using namespace lib::spt;

std::vector<track> api::api::top_tracks(const artist &artist)
{
	auto json = get(fmt::format("artists/{}/top-tracks?country=from_token", artist.id));
	return get_to_vector<track>(json.at("tracks"));
}

std::vector<Album> api::api::albums(const artist &artist)
{
	auto json = get(fmt::format("artists/{}/albums?country=from_token", artist.id));
	return get_to_vector<Album>(json.at("items"));
}

std::vector<artist> api::api::related_artists(const artist &artist)
{
	auto json = get(fmt::format("artists/{}/related-artists", artist.id));
	return get_to_vector<spt::artist>(json.at("artists"));
}