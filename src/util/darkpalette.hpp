#pragma once

#include <QPalette>
#include <QFile>
#include <QWidget>
#include <QFontDatabase>
#include <QApplication>

class DarkPalette: public QPalette
{
public:
	DarkPalette();

	static auto getDarkStylesheet() -> QString;
	static void addFonts();

private:
	static constexpr int fontPointSize = 10;
};
