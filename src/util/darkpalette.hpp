#pragma once

#include <QPalette>
#include <QFile>

class DarkPalette: public QPalette
{
public:
	DarkPalette();

	static auto getDarkStylesheet() -> QString;
};
