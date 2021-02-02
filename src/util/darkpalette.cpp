#include "darkpalette.hpp"

#include "../menu/mainmenu.hpp"

DarkPalette::DarkPalette()
{
	QColor background(0x212121);
	QColor base(0x282828);
	QColor highlight(0x1db954);
	QColor text(0xf5f5f5);
	QColor brightText(0xffffff);

	// Set colors
	setColor(QPalette::Window, background);
	setColor(QPalette::WindowText, text);
	setColor(QPalette::Base, base);
	setColor(QPalette::AlternateBase, background);
	setColor(QPalette::ToolTipBase, base);
	setColor(QPalette::ToolTipText, text);
	setColor(QPalette::Text, text);
	setColor(QPalette::Button, background);
	setColor(QPalette::ButtonText, text);
	setColor(QPalette::BrightText, brightText);
	setColor(QPalette::Highlight, highlight);
	setColor(QPalette::HighlightedText, base);
}
