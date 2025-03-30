#include "util/systeminfo.hpp"
#include "util/appinstalltype.hpp"

#include <QGuiApplication>

SystemInfo::SystemInfo()
{
	// spotify-qt version
#ifdef GIT_COMMIT
	add(QStringLiteral("App version"), QString("%1 (%2)")
		.arg(QCoreApplication::applicationVersion(), GIT_COMMIT));
#else
	add(QStringLiteral("App version"), QCoreApplication::applicationVersion());
#endif

	// Qt D-Bus support
#ifdef USE_DBUS
	add(QStringLiteral("D-Bus support"), QStringLiteral("Yes"));
#else
	add(QStringLiteral("D-Bus support"), QStringLiteral("No"));
#endif

	// App install type
	if (AppInstallType::get() != InstallType::Unknown)
	{
		add(QStringLiteral("Install type"), AppInstallType::getString());
	}

	// CMake build type
	add(QStringLiteral("Build type"), QStringLiteral(BUILD_TYPE));

	// Platform plugin
	add(QStringLiteral("Platform"), QGuiApplication::platformName());
}
