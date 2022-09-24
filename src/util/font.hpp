#pragma once

#include <QFont>

class Font
{
public:
	static auto monospace() -> QFont;

private:
	Font() = default;
};
