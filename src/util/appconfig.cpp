#include "util/appconfig.hpp"

#include <QApplication>
#include <QStyle>

auto AppConfig::useNativeMenuBar() -> bool
{
#ifdef __APPLE__
	return true;
#else
	return false;
#endif
}

auto AppConfig::useClickableSlider() -> bool
{
	const auto name = QApplication::style()->name();
	return name.compare(QStringLiteral("fusion"), Qt::CaseInsensitive) == 0;
}
