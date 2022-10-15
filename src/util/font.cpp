#include "util/font.hpp"

#include <QFontDatabase>
#include <QFontMetrics>

auto Font::monospace() -> QFont
{
	return QFontDatabase::systemFont(QFontDatabase::FixedFont);
}

auto Font::numberWidth() -> int
{
	QFont font;
	QFontMetrics metrics(font);
	auto max = 0;

	for (auto i = '0'; i <= '9'; i++)
	{
		max = std::max(max, metrics.boundingRect(i).width());
	}

	return max;
}
