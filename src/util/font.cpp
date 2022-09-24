#include "util/font.hpp"

auto Font::monospace() -> QFont
{
	QFont font(QStringLiteral("monospace"));
	font.setStyleHint(QFont::Monospace);
	return font;
}
