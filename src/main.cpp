#include "util/icon.hpp"
#include "util/appinstalltype.hpp"
#include "lib/qtpaths.hpp"

#include <QApplication>
#include <QCoreApplication>

#include "mainwindow.hpp"
#include "dialog/setup.hpp"

#ifdef USE_KCRASH
#include <kcrash.h>
#else
#include "lib/crash/crashhandler.hpp"
#endif

auto commandLineOptions() -> QList<QCommandLineOption>
{
	return {
		{
			QStringLiteral("dev"),
			QStringLiteral("Enable developer mode for troubleshooting issues."),
		},
		{
			QStringLiteral("reset-credentials"),
			QStringLiteral("Allows providing new Spotify credentials."),
		},
		{
			QStringLiteral("paths"),
			QStringLiteral("Print paths for config file and cache."),
		},
	};
}

auto main(int argc, char *argv[]) -> int
{
	// Set name for settings etc.
	QCoreApplication::setOrganizationName(ORG_NAME);
	QCoreApplication::setApplicationName(APP_NAME);
	QCoreApplication::setApplicationVersion(APP_VERSION);

	// Set installation type
	if (argc > 0)
	{
		const auto installType = AppInstallType::getInstallType(argv[0]);
		AppInstallType::set(installType);
	}

	// High-DPI support
	// These flags are deprecated and always enabled in Qt 6 and newer
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
	QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif

#ifndef USE_KCRASH
	// Custom crash handler if not using KCrash
	lib::crash_handler::init();
#endif

	// Create Qt application
	QApplication app(argc, argv);

	// Optional KCrash support
#ifdef USE_KCRASH
	KCrash::initialize();
	if (!KCrash::isDrKonqiEnabled())
	{
		lib::log::warn("Failed to initialize crash handler");
	}
#endif

	// Settings
	QtPaths paths(nullptr);
	lib::settings settings(paths);

	// Check fallback icons
	Icon::useFallbackIcons = settings.general.fallback_icons;

	// Show version if requested
	QCommandLineParser parser;
	parser.addVersionOption();
	parser.addHelpOption();
	parser.addOptions(commandLineOptions());
	parser.process(app);

	if (parser.isSet("dev"))
	{
		lib::developer_mode::enabled = true;
	}

	if (parser.isSet("paths"))
	{
		lib::log::info("Config: {}", paths.config_file().string());
		lib::log::info("Cache:  {}", paths.cache().string());
		return 0;
	}

	// First setup window
	if (settings.account.refresh_token.empty()
		|| parser.isSet("reset-credentials"))
	{
		Dialog::Setup dialog(settings, nullptr);
		if (dialog.exec() == QDialog::Rejected)
		{
			return 0;
		}
	}

	// Create main window
	MainWindow window(settings, paths);

	// Show window and run application
	if (!window.isValid())
	{
		return 1;
	}

	window.show();

	// Run application
	return QApplication::exec();
}
