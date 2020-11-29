#pragma once

#include "spotify.hpp"
#include "track.hpp"
#include "user.hpp"
#include "util/strings.hpp"

#include "lib/json.hpp"
#include "util/json.hpp"

namespace lib
{
	namespace spt
	{
		class Playlist
		{
		public:
			Playlist() = default;
			explicit Playlist(const nlohmann::json &json);

			std::vector<Track> loadTracks(Spotify &spotify) const;
			nlohmann::json toJson(Spotify &spotify) const;
			nlohmann::json toJson(const nlohmann::json &jsonTracks = nlohmann::json::array()) const;
			bool isOwner(const User &user) const;

			std::string description;
			std::string id;
			std::string image;
			std::string name;
			std::string snapshot;
			std::string ownerId;
			std::string ownerName;
			bool collaborative = false;
			bool isPublic = false;

		private:
			static bool loadTracksFromUrl(std::vector<Track> &trackList,
				std::string &url,
				int offset,
				Spotify &spotify);

			nlohmann::json tracks;
		};
	}
}