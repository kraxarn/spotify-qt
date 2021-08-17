#include "lib/spotify/playlist.hpp"

void lib::spt::to_json(nlohmann::json &j, const playlist &p)
{
	j = nlohmann::json{
		{"collaborative", p.collaborative},
		{"description", p.description},
		{"id", p.id},
		{"image", p.image},
		{"name", p.name},
		{"is_public", p.is_public},
		{"total", p.tracks.size()},
		{"tracks", p.tracks},
		{"snapshot", p.snapshot},
		{"owner_id", p.owner_id},
		{"owner_name", p.owner_name}
	};
}

void lib::spt::from_json(const nlohmann::json &j, playlist &p)
{
	if (!j.is_object())
	{
		return;
	}

	j.at("collaborative").get_to(p.collaborative);
	j.at("description").get_to(p.description);
	j.at("id").get_to(p.id);
	j.at("name").get_to(p.name);
	lib::json::get_property(j, {
		"is_public", "public"
	}, p.is_public);
	lib::json::get_property(j, {
		"snapshot", "snapshot_id"
	}, p.snapshot);

	if (j.contains("tracks"))
	{
		auto tracks = j.at("tracks");
		if (tracks.is_array())
		{
			tracks.get_to(p.tracks);
		}
		else if (tracks.is_object())
		{
			tracks.at("href").get_to(p.tracks_href);
			tracks.at("total").get_to(p.tracks_total);
		}
	}

	if (j.contains("image"))
	{
		j.at("image").get_to(p.image);
	}
	else if (j.contains("images"))
	{
		const auto &images = j.at("images");
		if (!images.empty())
		{
			images.front().at("url").get_to(p.image);
		}
	}

	if (j.contains("owner_id"))
	{
		j.at("owner_id").get_to(p.owner_id);
	}
	else if (j.contains("ownerId"))
	{
		j.at("ownerId").get_to(p.owner_id);
	}
	else
	{
		j.at("owner").at("id").get_to(p.owner_id);
	}

	if (j.contains("owner_name"))
	{
		j.at("owner_name").get_to(p.owner_name);
	}
	else if (j.contains("ownerName"))
	{
		j.at("ownerName").get_to(p.owner_name);
	}
	else
	{
		j.at("owner").at("display_name").get_to(p.owner_name);
	}
}

auto lib::spt::playlist::is_owner(const lib::spt::user &user) const -> bool
{
	return !owner_id.empty() && owner_id == user.id;
}

auto lib::spt::playlist::is_null() const -> bool
{
	return id.empty();
}

auto lib::spt::playlist::is_up_to_date(const std::string &playlist_snapshot) const -> bool
{
	return owner_id != "spotify"
		&& snapshot == playlist_snapshot;
}
