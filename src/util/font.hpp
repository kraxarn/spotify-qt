#pragma once

#include <QFont>
#include <QAction>

class Font
{
public:
	static auto monospace() -> QFont;
	static auto numberWidth() -> int;

	static void setFontWeight(QAction *action, QFont::Weight weight);

private:
	Font() = default;
};
