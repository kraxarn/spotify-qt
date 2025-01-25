#include "lib/spotify/track.hpp"
#include "lib/fmt.hpp"
#include "lib/json.hpp"
#include "lib/strings.hpp"

void lib::spt::to_json(nlohmann::json &j, const track &t)
{
	j = nlohmann::json{
		{"id", t.id},
		{"album", t.album},
		{"artists", t.artists},
		{"name", t.name},
		{"images", t.images},
		{"duration", t.duration},
		{"is_local", t.is_local},
		{"is_playable", t.is_playable},
		{"added_at", t.added_at}
	};
}

void from_cache(const nlohmann::json &j, lib::spt::track &t)
{
	if (!j.is_object())
	{
		return;
	}

	j.at("id").get_to(t.id);
	j.at("name").get_to(t.name);
	j.at("duration").get_to(t.duration);
	j.at("is_local").get_to(t.is_local);
	j.at("added_at").get_to(t.added_at);
	lib::json::get(j, "is_playable", t.is_playable);

	const auto &album = j.at("album");

	if (album.is_object())
	{
		album.get_to(t.album);
	}
	else if (album.is_string() && j.contains("album_id"))
	{
		album.get_to(t.album.name);
		j.at("album_id").get_to(t.album.id);
	}

	if (j.contains("artists"))
	{
		j.at("artists").get_to(t.artists);
	}
	else if (j.contains("artist"))
	{
		const auto &a = j.at("artist");
		lib::spt::entity artist;

		if (a.is_object())
		{
			a.get_to(artist);
		}
		else if (a.is_string() && j.contains("artist_id"))
		{
			a.get_to(artist.name);
			j.at("artist_id").get_to(artist.id);
		}

		t.artists.push_back(artist);
	}
	t.artists.shrink_to_fit();

	if (j.contains("image"))
	{
		lib::spt::image image;
		j.at("image").get_to(image.url);
		image.height = lib::spt::image::size_small;
		image.width = image.height;

		t.images.push_back(image);
	}
	else if (j.contains("images"))
	{
		j.at("images").get_to(t.images);
	}
	t.images.shrink_to_fit();
}

void lib::spt::from_json(const nlohmann::json &j, track &t)
{
	if (!j.is_object())
	{
		return;
	}

	// Duration in cache is "duration" and "duration_ms" from API
	if (j.contains("duration"))
	{
		from_cache(j, t);
		return;
	}

	// Object that contains the actual track object
	auto track = j.contains("track")
		? j.at("track")
		: j;

	lib::json::get(track, "id", t.id);
	lib::json::get(track, "name", t.name);
	lib::json::get(track, "duration_ms", t.duration);
	lib::json::get(track, "is_playable", t.is_playable);

	lib::json::get(j, "is_local", t.is_local);

	if (track.contains("artists"))
	{
		track.at("artists").get_to(t.artists);
		t.artists.shrink_to_fit();
	}

	if (track.contains("album"))
	{
		auto album = track.at("album");
		album.get_to(t.album);

		if (album.contains("images"))
		{
			const auto &images = album.at("images");
			if (images.is_array() && !images.empty())
			{
				images.get_to(t.images);
				t.images.shrink_to_fit();
			}
		}
	}

	if (j.contains("added_at"))
	{
		j.at("added_at").get_to(t.added_at);
	}
	else if (j.contains("played_at"))
	{
		j.at("played_at").get_to(t.added_at);
	}

	// Treat 1970-01-01 as no date
	if (lib::strings::starts_with(t.added_at, "1970-01-01"))
	{
		t.added_at = std::string();
	}
}

auto lib::spt::track::title() const -> std::string
{
	return is_valid()
		? lib::fmt::format("{} - {}", artists.front().name, name)
		: std::string("(no track)");
}

auto lib::spt::track::details() const -> std::string
{
	return is_valid()
		? lib::fmt::format("{}\n{}", name, entity::combine_names(artists))
		: std::string("(no track)");
}

auto lib::spt::track::image_small() const -> std::string
{
	for (const auto &image: images)
	{
		if (image.width == lib::spt::image::size_small
			|| image.height == lib::spt::image::size_small)
		{
			return image.url;
		}
	}

	return images.empty()
		? std::string()
		: images.back().url;
}

auto lib::spt::track::image_large() const -> std::string
{
	for (const auto &image: images)
	{
		if (image.width == lib::spt::image::size_large
			|| image.height == lib::spt::image::size_large)
		{
			return image.url;
		}
	}

	return images.empty()
		? std::string()
		: images.front().url;
}

auto lib::spt::track::is_valid() const -> bool
{
	return !artists.empty()
		&& !artists.front().name.empty()
		&& !name.empty();
}
