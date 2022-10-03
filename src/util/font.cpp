#include "util/font.hpp"

auto Font::monospace() -> QFont
{
	QFont font(QStringLiteral("monospace"));

	if (!font.fixedPitch())
	{
		font.setStyleHint(QFont::Monospace);
	}

	return font;
}
