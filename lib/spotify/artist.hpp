#pragma once

#include "album.hpp"
#include "spotify.hpp"
#include "track.hpp"

namespace lib
{
	namespace spt
	{
		class Artist
		{
		public:
			Artist() = default;
			explicit Artist(const nlohmann::json &json);

			std::vector<Track> topTracks(Spotify &spotify) const;
			std::vector<Album> albums(Spotify &spotify) const;
			std::vector<Artist> relatedArtists(Spotify &spotify) const;

			nlohmann::json toJson() const;

			int followers = 0;
			int popularity = 0;
			std::vector<std::string> genres;
			std::string id;
			std::string name;
			std::string image;
			std::vector<std::pair<std::string, std::string>> externalUrls;
		};
	}
}