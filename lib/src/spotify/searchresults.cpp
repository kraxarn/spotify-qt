#include "lib/spotify/searchresults.hpp"

void lib::spt::to_json(nlohmann::json &j, const search_results &s)
{
	j = nlohmann::json{
		{"albums", s.albums},
		{"artists", s.artists},
		{"tracks", s.tracks},
		{"playlists", s.playlists},
	};
}

void lib::spt::from_json(const nlohmann::json &j, search_results &s)
{
	if (!j.is_object())
	{
		return;
	}

	j.at("albums").at("items").get_to(s.albums);
	j.at("artists").at("items").get_to(s.artists);
	j.at("playlists").at("items").get_to(s.playlists);
	j.at("tracks").at("items").get_to(s.tracks);
}
