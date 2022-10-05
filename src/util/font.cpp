#include "util/font.hpp"

#include <QFontDatabase>

auto Font::monospace() -> QFont
{
	return QFontDatabase::systemFont(QFontDatabase::FixedFont);
}
