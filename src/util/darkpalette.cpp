#include "darkpalette.hpp"

DarkPalette::DarkPalette()
{
	// Pre-defined colors, maybe allow user to select
	QColor background(0x212121);
	QColor base(0x212121);
	QColor highlight(0x388e3c);
	QColor text(0xf5f5f5);
	QColor brightText(0xb00020);

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