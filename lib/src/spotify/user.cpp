#include "lib/spotify/user.hpp"

void lib::spt::to_json(nlohmann::json &j, const user &u)
{
	j = nlohmann::json{
		{"display_name", u.display_name},
		{"id", u.id},
		{"image", u.image},
		{"product", u.product}
	};
}

void lib::spt::from_json(const nlohmann::json &j, user &u)
{
	if (!j.is_object())
	{
		return;
	}

	j.at("display_name").get_to(u.display_name);
	j.at("id").get_to(u.id);
	j.at("product").get_to(u.product);

	const auto &images = j.at("images");
	if (!images.empty())
	{
		images.back().at("url").get_to(u.image);
	}
}
