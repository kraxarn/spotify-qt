#include "lib/format.hpp"
#include "lib/fmt.hpp"
#include "lib/time.hpp"

auto Format::time(const int milliseconds) -> QString
{
	const int total_seconds = milliseconds / lib::time::ms_in_sec;

	const int minutes = total_seconds / lib::time::secs_in_min;
	const int seconds = total_seconds % lib::time::secs_in_min;

	const QString seconds_prefixed = QStringLiteral("%1%2")
		.arg(seconds < 10 ? QStringLiteral("0") : QString())
		.arg(seconds);

	return QStringLiteral("%1:%2")
		.arg(minutes)
		.arg(seconds_prefixed);
}

auto Format::timePretty(const int milliseconds) -> QString
{
	const int minutes = milliseconds / lib::time::ms_in_min;
	if (minutes < lib::time::min_in_hour)
	{
		return QStringLiteral("%1 m").arg(minutes);
	}

	const int hours = minutes / lib::time::min_in_hour;
	return minutes == lib::time::min_in_hour
		? QStringLiteral("%1 h").arg(hours)
		: QStringLiteral("%1 h %2 m").arg(hours).arg(minutes % lib::time::min_in_hour);
}

auto Format::size(const unsigned long bytes) -> QString
{
	if (bytes >= giga)
	{
		return QStringLiteral("%1 GB").arg(bytes / giga);
	}

	if (bytes >= mega)
	{
		return QStringLiteral("%1 MB").arg(bytes / mega);
	}

	if (bytes >= kilo)
	{
		return QStringLiteral("%1 kB").arg(bytes / kilo);
	}

	return QStringLiteral("%1 B").arg(bytes);
}

auto Format::count(const unsigned int count) -> QString
{
	if (count >= mega)
	{
		return QStringLiteral("%1M").arg(count / mega);
	}

	if (count >= kilo)
	{
		return QStringLiteral("%1k").arg(count / kilo);
	}

	return QString::number(count);
}

auto lib::format::title(const spt::track &track, const std::string &format) -> std::string
{
	std::string result;
	size_t start_index = 0;

	if (format.empty())
	{
		return track.title();
	}

	while (true)
	{
		const auto prev_start_index = start_index;
		start_index = format.find('{', start_index);
		result.append(format.substr(prev_start_index, start_index - prev_start_index));

		if (start_index == std::string::npos)
		{
			break;
		}

		const auto end_index = format.find('}', start_index);
		if (end_index == std::string::npos)
		{
			return format;
		}

		const auto part = format.substr(start_index, end_index - start_index + 1);

		if (part == "{track}")
		{
			result.append(track.name);
		}
		else if (part == "{artist}")
		{
			if (!track.artists.empty())
			{
				result.append(track.artists.at(0).name);
			}
		}
		else if (part == "{artists}")
		{
			for (size_t i = 0; i < track.artists.size(); i++)
			{
				result.append(track.artists.at(i).name);
				if (i < track.artists.size() - 1)
				{
					result.append(", ");
				}
			}
		}
		else
		{
			result.append(part);
		}

		start_index = end_index + 1;
	}

	return result;
}
