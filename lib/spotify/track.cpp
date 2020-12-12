#include "track.hpp"

using namespace lib::spt;

track::track(const nlohmann::json &item)
{
	// If album is a string, track is loaded from cache
	if (item.at("album").is_string())
	{
		item.at("id").get_to(id);
		item.at("album").get_to(album);
		item.at("artist").get_to(artist);
		item.at("name").get_to(name);
		item.at("duration").get_to(duration);
		item.at("image").get_to(image);
		item.at("is_playable").get_to(is_playable);

		lib::json::get_property(item, {
			"album_id", "albumId"
		}).get_to(album_id);

		lib::json::get_property(item, {
			"artist_id", "artistId"
		}).get_to(artist_id);

		lib::json::get_property(item, {
			"is_local", "isLocal"
		}).get_to(is_local);

		added_at = date::parse(lib::json::get_property(item, {
			"added_at", "addedAt"
		}).get<std::string>());

		return;
	}

	// Object that contains the actual track object
	auto track = item.contains("track")
		? item.at("track")
		: item;

	// All properties
	id = lib::json::get_or_default<std::string>(track, "id", "0");
	name = lib::json::get_or_default<std::string>(track, "name", "(no name)");
	duration = lib::json::get_or_default(track, "duration_ms", 0);

	auto jsonAlbum = item.at("album");
	album = lib::json::get_or_default<std::string>(jsonAlbum, "name", "(no album)");
	album_id = lib::json::get_or_default<std::string>(jsonAlbum, "id", "0");

	jsonAlbum.at("images").at(2).at("url").get_to(image);

	auto jsonArtist = item.at("artists").at(0);
	artist = lib::json::get_or_default<std::string>(jsonArtist, "name", "(no artist)");
	artist_id = lib::json::get_or_default<std::string>(jsonArtist, "id", "0");

	added_at = item.contains("added_at")
		? date::parse(item.at("added_at").get<std::string>())
		: item.contains("played_at")
			? date::parse(item.at("played_at").get<std::string>())
			: date();

	item.at("is_local").get_to(is_local);
	track.at("is_playable").get_to(is_playable);
}

nlohmann::json track::to_json() const
{
	return {
		{"id", id},
		{"album", album},
		{"album_id", album_id},
		{"artist", artist},
		{"artist_id", artist_id},
		{"name", name},
		{"image", image},
		{"duration", duration},
		{"is_local", is_local},
		{"is_playable", is_playable},
		{"added_at", added_at.to_iso_date_time()}
	};
}