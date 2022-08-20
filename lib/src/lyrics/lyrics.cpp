#include "lib/lyrics/lyrics.hpp"
#include "lib/strings.hpp"

void lib::lrc::from_json(const nlohmann::json &json, lib::lrc::lyrics &lyrics)
{
	if (!json.is_object())
	{
		return;
	}

	const auto lines = lib::strings::split(json, '\n');
	lyrics.lines.reserve(lines.size());

	for (const auto &line: lines)
	{
		lyrics.lines.emplace_back(line);
	}
}
