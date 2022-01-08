#include "lib/search/results.hpp"

void lib::ddg::from_json(const nlohmann::json &j, results &r)
{
	if (!j.is_object())
	{
		return;
	}

	j.at("AbstractSource").get_to(r.abstract_source);
	j.at("AbstractURL").get_to(r.abstract_url);

	const auto &infobox = j.at("Infobox");
	if (infobox.is_object())
	{
		infobox.at("content").get_to(r.content);
		infobox.at("meta").get_to(r.meta);
	}
}
