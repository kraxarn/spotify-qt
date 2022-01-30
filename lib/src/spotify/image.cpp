#include "lib/spotify/image.hpp"

void lib::spt::from_json(const nlohmann::json &j, image &i)
{
	if (!j.is_object())
	{
		return;
	}

	j.at("url").get_to(i.url);
	j.at("height").get_to(i.height);
	j.at("width").get_to(i.width);
}

void lib::spt::to_json(nlohmann::json &j, const image &i)
{
	j = nlohmann::json{
		{"url", i.url},
		{"height", i.height},
		{"width", i.width},
	};
}
