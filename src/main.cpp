#include "lib/qtpaths.hpp"
#include "lib/spotify/request.hpp"
#include "spotify/deviceselect.hpp"
#include "util/appinstalltype.hpp"
#include "util/icon.hpp"

#include <QApplication>
#include <QCoreApplication>

#include "mainwindow.hpp"
#include "commandline/args.hpp"
#include "commandline/parser.hpp"
#include "commandline/processor.hpp"
#include "dialog/setup.hpp"
#include "lib/crash/crashhandler.hpp"
#include "util/refresher.hpp"

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

	// Custom crash handler
	lib::crash_handler::init();

	// Create Qt application
	QApplication app(argc, argv);

	// Settings
	QtPaths paths(nullptr);
	lib::settings settings(paths);

	// Check fallback icons
	Icon::useFallbackIcons = settings.general.fallback_icons;

	// Command line arguments
	CommandLine::Parser parser(app);

	if (CommandLine::Processor::process(parser))
	{
		return 0;
	}

	if (parser.isSet(ARG_LIST_PATHS))
	{
		lib::log::info("Config: {}", paths.config_file().string());
		lib::log::info("Cache:  {}", paths.cache().string());
		return 0;
	}

	// First setup window
	if (settings.account.refresh_token.empty() || parser.isSet(ARG_FORCE_SETUP))
	{
		Dialog::Setup dialog(settings, nullptr);
		if (dialog.exec() == QDialog::Rejected)
		{
			return 0;
		}
	}

	lib::qt::http_client httpClient(nullptr);
	const spt::DeviceSelect deviceSelect(nullptr);
	lib::spt::request request(settings, httpClient, deviceSelect);
	lib::spt::api spotify(settings, httpClient, request);

	Refresher refresher(settings, request);
	if (!refresher.refresh())
	{
		return 1;
	}

	MainWindow window(settings, paths, httpClient, spotify);
	window.show();

	return QApplication::exec();
}
