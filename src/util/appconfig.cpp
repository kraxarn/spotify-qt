#include "util/appconfig.hpp"
#include <QCoreApplication>

auto AppConfig::useNativeMenuBar() -> bool
{
	return !QCoreApplication::testAttribute(Qt::AA_DontUseNativeMenuBar);
}
