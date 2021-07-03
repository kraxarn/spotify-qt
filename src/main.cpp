#include "util/icon.hpp"
#include "lib/qtpaths.hpp"

#include <QApplication>
#include <QCoreApplication>

#include "mainwindow.hpp"
#include "dialog/setupdialog.hpp"

#ifdef USE_KCRASH
#include <kcrash.h>
#else
#include "lib/crash/crashhandler.hpp"
#endif

auto main(int argc, char *argv[]) -> int
{
	// Set name for settings etc.
	QCoreApplication::setOrganizationName("kraxarn");
	QCoreApplication::setApplicationName("spotify-qt");
	QCoreApplication::setApplicationVersion(APP_VERSION);

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
	parser.addOptions({
		{"dev", "Enable developer mode for troubleshooting issues."},
		{"reset-credentials", "Allows providing new Spotify credentials."}
	});
	parser.process(app);

	if (parser.isSet("dev"))
	{
		lib::developer_mode::enabled = true;
	}

	// First setup window
	if (settings.account.refresh_token.empty()
		|| parser.isSet("reset-credentials"))
	{
		SetupDialog dialog(settings);
		if (dialog.exec() == QDialog::Rejected)
		{
			return 0;
		}
	}

	// Create main window
	MainWindow w(settings, paths);

	// Show window and run application
	if (!w.isValid())
	{
		return 1;
	}

	w.show();

	// Run application
	return QApplication::exec();
}
