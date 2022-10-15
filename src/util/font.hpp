#pragma once

#include <QFont>

class Font
{
public:
	static auto monospace() -> QFont;
	static auto numberWidth() -> int;

private:
	Font() = default;
};
