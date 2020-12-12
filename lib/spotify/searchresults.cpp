#include "searchresults.hpp"

using namespace lib::spt;

SearchResults::SearchResults(const nlohmann::json &json)
{
	// Albums
	albums.reserve(json.at("albums").at("total").get<int>());
	for (auto &album : json.at("albums").at("items").items())
		albums.emplace_back(album.value());

	// Artists
	artists.reserve(json.at("artists").at("total").get<int>());
	for (auto &artist : json.at("artists").at("items").items())
		artists.emplace_back(artist.value());

	// Playlists
	playlists.reserve(json.at("playlists").at("total").get<int>());
	for (auto &playlist : json.at("playlists").at("items").items())
		playlists.emplace_back(playlist.value());

	// Tracks
	tracks.reserve(json.at("tracks").at("total").get<int>());
	for (auto &track : json.at("tracks").at("items").items())
		tracks.emplace_back(track(track.value()));
}

nlohmann::json SearchResults::toJson() const
{
	nlohmann::json jsonAlbums;
	for (auto &album : albums)
		jsonAlbums.push_back(album.toJson());

	nlohmann::json jsonArtists;
	for (auto &artist : artists)
		jsonArtists.push_back(artist.to_json());

	nlohmann::json jsonTracks;
	for (auto &track : tracks)
		jsonTracks.push_back(track.to_json());

	nlohmann::json jsonPlaylists;
	for (auto &playlist : playlists)
		jsonPlaylists.push_back(playlist);

	return {
		{"albums", jsonAlbums},
		{"artists", jsonArtists},
		{"tracks", jsonTracks},
		{"playlists", jsonPlaylists}
	};
}
