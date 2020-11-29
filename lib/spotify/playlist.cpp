#include "playlist.hpp"

using namespace lib::spt;

Playlist::Playlist(const nlohmann::json &json)
{
	json.at("collaborative").get_to(collaborative);
	json.at("description").get_to(description);
	json.at("id").get_to(id);
	json.at("name").get_to(name);
	json.at("tracks").get_to(tracks);

	(json.contains("image")
		? json.at("image")
		: json.at("images").at(0).at("url")).get_to(image);

	lib::json::get_property(json, {
		"is_public", "public"
	}).get_to(isPublic);

	lib::json::get_property(json, {
		"snapshot", "snapshot_id"
	}).get_to(snapshot);

	(json.contains("owner_id")
		? json.at("owner_id")
		: json.contains("ownerId")
			? json.at("ownerId")
			: json.at("owner").at("id")).get_to(ownerId);

	(json.contains("owner_name")
		? json.at("owner_name")
		: json.contains("ownerName")
			? json.at("ownerName")
			: json.at("owner").at("display_name")).get_to(ownerName);
}

std::vector<Track> Playlist::loadTracks(Spotify &spotify) const
{
	// Allocate memory for all tracks
	std::vector<Track> trackList;
	trackList.reserve(tracks.at("total").get<int>());

	// Load tracks
	auto tracksHref = tracks.at("href").get<std::string>();
	auto href = fmt::format("{}{}market=from_token",
		tracksHref, strings::contains(tracksHref, '?') ? '&' : '?');
	loadTracksFromUrl(trackList, href, 0, spotify);

	// Return final track list
	return trackList;
}

bool Playlist::loadTracksFromUrl(std::vector<Track> &trackList, std::string &url, int offset, Spotify &spotify)
{
	// Load tracks from api
	auto newUrl = url.erase(0, std::string("https://api.spotify.com/v1/").length());
	auto current = spotify.getAsObject(newUrl);

	// Load from url
	auto items = current.at("items");
	for (auto item : items.items)
		trackList.push_back(Track(item.toObject()));

	// Check if there's a next page
	auto nextPage = current.at("next").get<std::string>();
	if (!nextPage.is_empty())
		loadTracksFromUrl(trackList, nextPage, offset + items.size(), spotify);

	return true;
}

nlohmann::json Playlist::toJson(Spotify &spotify) const
{
	// Load tracks to put in JSON
	auto jsonTracks = nlohmann::json::array();
	for (auto &track : loadTracks(spotify))
		jsonTracks.push_back(track.toJson());

	return toJson(jsonTracks);
}

bool Playlist::isOwner(const User &user) const
{
	return !ownerId.empty() && ownerId == user.id;
}

nlohmann::json Playlist::toJson(const nlohmann::json &jsonTracks) const
{
	return {
		{"collaborative", collaborative},
		{"description", description},
		{"id", id},
		{"image", image},
		{"name", name},
		{"is_public", isPublic},
		{"total", jsonTracks.size()},
		{"tracks", jsonTracks},
		{"snapshot", snapshot},
		{"owner_id", ownerId},
		{"owner_name", ownerName}
	};
