#include "lib/spotify/playlistdetails.hpp"

lib::spt::playlist_details::playlist_details(const lib::spt::playlist &playlist)
{
	name = playlist.name;
	is_public = playlist.is_public;
	collaborative = playlist.collaborative;
	description = playlist.description;
}

void lib::spt::to_json(nlohmann::json &j, const playlist_details &p)
{
	j = nlohmann::json{
		{"name", p.name},
		{"public", p.is_public},
		{"collaborative", p.collaborative},
		{"description", p.description},
	};
}

void lib::spt::from_json(const nlohmann::json &j, playlist_details &p)
{
	if (!j.is_object())
	{
		return;
	}

	j.at("name").get_to(p.name);
	lib::json::get_property(j, {
		"is_public", "public"
	}, p.is_public);
	j.at("collaborative").get_to(p.collaborative);
	j.at("description").get_to(p.description);
}
