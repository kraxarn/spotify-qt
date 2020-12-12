#include "album.hpp"

using namespace lib::spt;

Album::Album(const nlohmann::json &json)
{
	json.at("id").get_to(id);
	json.at("album_group").get_to(albumGroup);
	json.at("images").at(2).at("url").get_to(image);
	json.at("name").get_to(name);
	json.at("artists").at(0).at("name").get_to(artist);

	releaseDate = date::parse(json.at("release_date").get<std::string>());
}

nlohmann::json Album::toJson() const
{
	return {
		{"id", id},
		{"album_group", albumGroup},
		{"image", image},
		{"name", name},
		{"artist", artist},
		{"release_date", releaseDate.to_iso_date()},
	};
}