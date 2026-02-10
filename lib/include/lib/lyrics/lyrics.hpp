#pragma once

#include "lib/lyrics/line.hpp"

#include "thirdparty/json.hpp"

#include <QList>

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
		QList<LyricsLine> syncedLyrics;
	};

	void from_json(const nlohmann::json &json, lyrics &lyrics);
}
