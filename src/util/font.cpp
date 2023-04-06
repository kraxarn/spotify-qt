#include "util/font.hpp"

#include <QFontDatabase>
#include <QFontMetrics>
#include <QWidget>

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

void Font::setFontWeight(QWidget *widget, QFont::Weight weight)
{
	auto font = widget->font();
	font.setWeight(weight);
	widget->setFont(font);
}
