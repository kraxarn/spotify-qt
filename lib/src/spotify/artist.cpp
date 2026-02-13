#include "lib/spotify/artist.hpp"

void lib::spt::to_json(nlohmann::json &j, const artist &a)
{
	j = nlohmann::json{
		{"genres", a.genres},
		{"id", a.id},
		{"name", a.name},
		{"image", a.image}
	};

	if (a.followers >= 0)
	{
		j["followers"] = a.followers;
	}

	if (a.popularity >= 0)
	{
		j["popularity"] = a.popularity;
	}
}

void lib::spt::from_json(const nlohmann::json &j, artist &a)
{
	if (!j.is_object())
	{
		return;
	}

	j.at("id").get_to(a.id);
	j.at("name").get_to(a.name);
	j.at("external_urls").get_to(a.external_urls);

	if (j.contains("popularity"))
	{
		j.at("popularity").get_to(a.popularity);
	}

	if (j.contains("genres"))
	{
		j.at("genres").get_to(a.genres);
	}

	if (j.contains("followers"))
	{
		auto followers = j.at("followers");

		if (followers.is_object())
		{
			followers.at("total").get_to(a.followers);
		}
		else if (followers.is_number_integer())
		{
			followers.get_to(a.followers);
		}
	}

	if (j.contains("image"))
	{
		j.at("image").get_to(a.image);
	}
	else if (j.contains("images"))
	{
		const auto &images = j.at("images");
		if (images.size() > 1)
		{
			images.at(1).at("url").get_to(a.image);
		}
		else if (!images.empty())
		{
			images.front().at("url").get_to(a.image);
		}
	}
}
