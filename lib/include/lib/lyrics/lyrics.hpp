#pragma once

#include "lib/lyrics/line.hpp"

#include "thirdparty/json.hpp"

namespace lib::lrc
{
	class lyrics
	{
	public:
		unsigned int id;
		std::string track_name;
		std::string artist_name;
		std::string album_name;
		unsigned int duration;
		bool instrumental;

		/**
		 * Plain text lyrics, one string per line
		 */
		std::vector<std::string> plain_lyrics;

		/**
		 * Synced lyrics with timestamps
		 */
		std::vector<line> synced_lyrics;
	};

	void from_json(const nlohmann::json &json, lyrics &lyrics);
}
