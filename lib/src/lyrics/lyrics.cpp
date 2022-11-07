#include "lib/lyrics/lyrics.hpp"
#include "lib/strings.hpp"
#include "lib/vector.hpp"

void lib::lrc::from_json(const nlohmann::json &json, lib::lrc::lyrics &lyrics)
{
	if (!json.is_string())
	{
		return;
	}

	const auto lines = lib::strings::split(json, '\n');
	lyrics.lines.reserve(lines.size());

	auto iter = lines.cbegin();
	while (iter != lines.cend())
	{
		const lib::lrc::line parsed(*(iter++));
		if (parsed.text.find(':') == std::string::npos)
		{
			iter--;
			break;
		}
		lyrics.credits.emplace_back(parsed.text);
	}

	lib::vector::unique(lyrics.credits);

	while (iter != lines.cend())
	{
		const lib::lrc::line parsed(*(iter++));
		lyrics.lines.push_back(parsed);
	}

	lyrics.credits.emplace_back(":Provided by NetEase");
}

auto lib::lrc::lyrics::is_synced() const -> bool
{
	return std::any_of(lines.cbegin(), lines.cend(), [](const lib::lrc::line &line) -> bool
	{
		return line.timestamp >= 0L;
	});
}
