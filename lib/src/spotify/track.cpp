#include "lib/spotify/track.hpp"

#include "lib/json.hpp"
#include "lib/strings.hpp"

void lib::spt::to_json(nlohmann::json &j, const track &t)
{
	j = nlohmann::json{
		{"id", t.id},
		{"album", t.album},
		{"album_id", t.album_id},
		{"artist", t.artist},
		{"artist_id", t.artist_id},
		{"name", t.name},
		{"image", t.image},
		{"duration", t.duration},
		{"is_local", t.is_local},
		{"is_playable", t.is_playable},
		{"added_at", t.added_at}
	};
}

void lib::spt::from_json(const nlohmann::json &j, track &t)
{
	if (!j.is_object())
		return;

	// If album is a string, track is loaded from cache
	if (j.contains("album") && j.at("album").is_string())
	{
		j.at("id").get_to(t.id);
		j.at("album").get_to(t.album);
		j.at("album_id").get_to(t.album_id);
		j.at("artist").get_to(t.artist);
		j.at("artist_id").get_to(t.artist_id);
		j.at("name").get_to(t.name);
		j.at("duration").get_to(t.duration);
		j.at("image").get_to(t.image);
		j.at("is_local").get_to(t.is_local);
		j.at("added_at").get_to(t.added_at);
		j.at("is_playable").get_to(t.is_playable);
		return;
	}

	// Object that contains the actual track object
	auto track = j.contains("track")
		? j.at("track")
		: j;

	lib::json::get(track, "id", t.id);
	lib::json::get(track, "name", t.name);
	lib::json::get(track, "duration_ms", t.duration);
	lib::json::get(track, "is_local", t.is_local);
	lib::json::get(track, "is_playable", t.is_playable);

	if (track.contains("artists"))
	{
		auto artist = track.at("artists").at(0);
		artist.at("name").get_to(t.artist);
		artist.at("id").get_to(t.artist_id);
	}
	else
	{
		t.artist = "(no artist)";
		t.artist_id = "0";
	}

	if (track.contains("album"))
	{
		auto album = track.at("album");
		album.at("id").get_to(t.album_id);
		album.at("name").get_to(t.album);
		album.at("images").back().at("url").get_to(t.image);
	}
	else
	{
		t.album = "(no album)";
		t.album_id = "0";
	}

	if (track.contains("added_at"))
		track.at("added_at").get_to(t.added_at);
	else if (track.contains("played_ar"))
		track.at("played_at").get_to(t.added_at);

	// Treat 1970-01-01 as no date
	if (lib::strings::starts_with(t.added_at, "1970-01-01"))
		t.added_at = std::string();
}

std::string lib::spt::track::title()
{
	return lib::fmt::format("{} - {}", artist, name);
}
