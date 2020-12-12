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