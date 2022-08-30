#include "lib/lyrics/line.hpp"

lib::lrc::line::line(const std::string &line)
	: data(line)
{
	const auto timestamp_start = line.find('[');
	const auto timestamp_end = line.find(']');

	if (timestamp_start == std::string::npos
		|| timestamp_end == std::string::npos)
	{
		timestamp = -1L;
	}
	else
	{
		const auto length = timestamp_end - timestamp_start - 2;
		timestamp = parse_timestamp(line.substr(timestamp_start + 1, length));
	}

	if (line.empty() || timestamp_end == line.size() - 1)
	{
		text = "♪";
	}
	else if (timestamp_end != std::string::npos && timestamp > 0)
	{
		text = line.substr(timestamp_end + 1);
	}
	else
	{
		text = line;
	}
}

auto lib::lrc::line::parse_timestamp(const std::string &timestamp) -> long
{
	// minute:second.millisecond

	const auto second = timestamp.find(':');
	const auto millisecond = timestamp.find('.');

	if (second == std::string::npos
		|| millisecond == std::string::npos)
	{
		return -1L;
	}

	const auto minutes_str = timestamp.substr(0, 2);
	const auto seconds_str = timestamp.substr(second + 1, 2);
	const auto milliseconds_str = timestamp.substr(millisecond + 1, 3);

	const auto minutes = minutes_str.empty() ? 0 : std::stoi(minutes_str);
	const auto seconds = seconds_str.empty() ? 0 : std::stoi(seconds_str);
	const auto milliseconds = milliseconds_str.empty() ? 0 : std::stoi(milliseconds_str);

	constexpr int milliseconds_in_second = 1000;
	constexpr int milliseconds_in_minute = milliseconds_in_second * 60;

	return minutes * milliseconds_in_minute
		+ seconds * milliseconds_in_second
		+ milliseconds;
}
