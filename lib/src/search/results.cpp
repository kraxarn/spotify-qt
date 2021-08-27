#include "lib/search/results.hpp"

void lib::ddg::from_json(const nlohmann::json &j, results &r)
{
	if (!j.is_object())
	{
		return;
	}

	const auto &infobox = j.at("Infobox");
	if (infobox.is_object())
	{
		infobox.at("content").get_to(r.content);
		infobox.at("meta").get_to(r.meta);
	}
}
