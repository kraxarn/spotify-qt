#include "util/systeminfo.hpp"
#include "util/appinstalltype.hpp"

SystemInfo::SystemInfo()
{
	// spotify-qt version
#ifdef GIT_COMMIT
	add(QStringLiteral("App version"), QString("%1 (%2)")
		.arg(APP_VERSION, GIT_COMMIT));
#else
	add(QStringLiteral("App version"), APP_VERSION);
#endif

	// Qt D-Bus support
#ifdef USE_DBUS
	add(QStringLiteral("D-Bus support"), QStringLiteral("Yes"));
#else
	add(QStringLiteral("D-Bus support"), QStringLiteral("No"));
#endif

	// KCrash support
#ifdef USE_KCRASH
	add(QStringLiteral("KCrash support"), QStringLiteral("Yes"));
#else
	add(QStringLiteral("KCrash support"), QStringLiteral("No"));
#endif

	// App install type
	if (AppInstallType::get() != InstallType::Unknown)
	{
		add(QStringLiteral("Install type"), AppInstallType::getString());
	}

	// CMake build type
	add(QStringLiteral("Build type"), QStringLiteral(BUILD_TYPE));
}
