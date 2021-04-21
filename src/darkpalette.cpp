#include "darkpalette.hpp"

DarkPalette::DarkPalette()
{
	constexpr unsigned int backgroundColor = 0x212121;
	constexpr unsigned int baseColor = 0x282828;
	constexpr unsigned int highlightColor = 0x1db954;
	constexpr unsigned int textColor = 0xf5f5f5;
	constexpr unsigned int brightTextColor = 0xffffff;

	QColor background(backgroundColor);
	QColor base(baseColor);
	QColor highlight(highlightColor);
	QColor text(textColor);
	QColor brightText(brightTextColor);

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
