#include "lib/spotify/album.hpp"

void lib::spt::to_json(nlohmann::json &j, const album &a)
{
	j = nlohmann::json{
		{"id", a.id},
		{"image", a.image},
		{"name", a.name},
		{"artist", a.artist},
		{"release_date", a.release_date},
	};

	if (a.album_group != lib::album_group::none)
	{
		std::string album_group;

		switch (a.album_group)
		{
			case album_group::album:
				album_group = "album";
				break;

			case album_group::single:
				album_group = "single";
				break;

			case album_group::compilation:
				album_group = "compilation";
				break;

			case album_group::appears_on:
				album_group = "appears_on";
				break;

			case album_group::none:
				album_group = "none";
				break;
		}

		j.at("album_group") = album_group;
	}
}

void lib::spt::from_json(const nlohmann::json &j, album &a)
{
	if (!j.is_object())
	{
		return;
	}

	j.at("id").get_to(a.id);
	j.at("name").get_to(a.name);
	j.at("release_date").get_to(a.release_date);

	if (j.contains("album_group"))
	{
		auto album_group = j.at("album_group").get<std::string>();
		a.album_group = album_group == "album"
			? lib::album_group::album
			: album_group == "single"
				? lib::album_group::single
				: album_group == "compilation"
					? lib::album_group::compilation
					: album_group == "appears_on"
						? lib::album_group::appears_on
						: lib::album_group::none;
	}

	if (j.contains("images"))
	{
		const auto &images = j.at("images");
		if (images.is_array() && !images.empty())
		{
			images.back().at("url").get_to(a.image);
		}
	}
	else if (j.contains("image"))
	{
		j.at("image").get_to(a.image);
	}

	if (j.contains("artists"))
	{
		j.at("artists").front().at("name").get_to(a.artist);
	}
	else if (j.contains("artist"))
	{
		j.at("artist").get_to(a.artist);
	}
}
