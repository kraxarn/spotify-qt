#include "lib/lyrics/lyrics.hpp"
#include "lib/strings.hpp"

void lib::lrc::from_json(const nlohmann::json &json, lib::lrc::lyrics &lyrics)
{
	if (!json.is_string())
	{
		return;
	}

	const auto lines = lib::strings::split(json, '\n');
	lyrics.lines.reserve(lines.size());

	for (const auto &line: lines)
	{
		const lib::lrc::line parsed(line);

		if (lyrics.lines.empty()
			&& (parsed.timestamp < 0 || parsed.timestamp % 1000L == 0)
			&& parsed.text.find(':') != std::string::npos)
		{
			lyrics.credits.emplace_back(parsed.text);
			continue;
		}

		lyrics.lines.push_back(parsed);
	}

	lyrics.credits.emplace_back(lib::lyrics_credit::provided, "NetEase");
}
