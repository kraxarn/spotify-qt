#include "lib/lyrics/lyrics.hpp"
#include "lib/log.hpp"
#include "lib/strings.hpp"

void lib::lrc::from_json(const nlohmann::json &json, lyrics &lyrics)
{
	if (!json.is_object())
	{
		return;
	}

	json.at("id").get_to(lyrics.id);
	json.at("trackName").get_to(lyrics.track_name);
	json.at("artistName").get_to(lyrics.artist_name);
	json.at("albumName").get_to(lyrics.album_name);
	json.at("duration").get_to(lyrics.duration);
	json.at("instrumental").get_to(lyrics.instrumental);

	if (const auto &obj = json.at("plainLyrics"); obj.is_string())
	{
		const auto &plain_lyrics = obj.get<std::string>();
		lyrics.plain_lyrics = strings::split(plain_lyrics, '\n');
	}

	if (const auto &obj = json.at("syncedLyrics"); obj.is_string())
	{
		const auto synced_lyrics = obj.get<std::string>();
		const auto lines = strings::split(synced_lyrics, '\n');

		lyrics.syncedLyrics.reserve(static_cast<qsizetype>(lines.size()));

		auto iter = lines.cbegin();
		while (iter != lines.cend())
		{
			try
			{
				const LyricsLine parsed(QString::fromStdString(*iter));
				lyrics.syncedLyrics.append(parsed);
				++iter;
			}
			catch (const std::exception &e)
			{
				log::warn("Ignoring invalid line '{}': {}", *iter, e.what());
				++iter;
			}
		}
	}
}
