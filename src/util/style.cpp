#include "util/style.hpp"

bool Style::darkBackground = false;

void Style::applyPalette(lib::palette palette)
{
	QPalette p;
	switch (palette)
	{
		case lib::palette::app:
			p = QApplication::palette();
			break;

		case lib::palette::style:
			p = QApplication::style()->standardPalette();
			break;

		case lib::palette::dark:
			p = DarkPalette();
			break;
	}

	QApplication::setPalette(p);
}

void Style::setDarkBackground(QWidget *widget)
{
	const auto bg = widget->palette().color(widget->backgroundRole());
	constexpr auto threshold = 128;
	darkBackground = ((bg.red() + bg.green() + bg.blue()) / 3) < threshold;
}

auto Style::getDarkBackground() -> bool
{
	return darkBackground;
}

void Style::apply(QWidget *window, lib::settings &settings)
{
	// Apply selected style and palette
	QApplication::setStyle(QString::fromStdString(settings.general.style));
	Style::applyPalette(settings.general.style_palette);

	if (settings.general.style_palette == lib::palette::dark)
	{
		// Custom dark theme
		window->setStyleSheet(DarkPalette::getDarkStylesheet());
		darkBackground = true;
	}
	else
	{
		// Check for dark background
		setDarkBackground(window);
	}

	// Custom font
	const auto &qtSettings = settings.qt();
	if (!qtSettings.custom_font_name.empty())
	{
		const auto fontName = QString::fromStdString(qtSettings.custom_font_name);
		QApplication::setFont(qtSettings.custom_font_size > 0
			? QFont(fontName, qtSettings.custom_font_size)
			: QFont(fontName));
	}
}
