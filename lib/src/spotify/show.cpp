#include "lib/spotify/show.hpp"

void lib::spt::from_json(const nlohmann::json &j, show &s)
{
	if (!j.is_object())
	{
		return;
	}

	j.at("available_markets").get_to(s.available_markets);
	j.at("description").get_to(s.description);
	j.at("explicit").get_to(s.is_explicit);
	j.at("external_urls").get_to(s.external_urls);
	j.at("href").get_to(s.href);
	j.at("html_description").get_to(s.html_description);
	j.at("id").get_to(s.id);
	j.at("is_externally_hosted").get_to(s.is_externally_hosted);
	j.at("languages").get_to(s.languages);
	j.at("name").get_to(s.name);
	j.at("publisher").get_to(s.publisher);
	j.at("uri").get_to(s.uri);

	lib::json::find_item("image", j, s.image);

	const auto &media_type = j.at("media_type").get<std::string>();
	s.media_type = lib::enums<lib::media_type>::parse(media_type);
}
