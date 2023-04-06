#pragma once

#include <QFont>

class Font
{
public:
	static auto monospace() -> QFont;
	static auto numberWidth() -> int;

	static void setFontWeight(QWidget *widget, QFont::Weight weight);

private:
	Font() = default;
};
