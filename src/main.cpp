#include "appversion.hpp"
#include "dialog/setupdialog.hpp"
#include "lib/qtpaths.hpp"
#include "mainwindow.hpp"
#include "util/icon.hpp"

#include <QApplication>
#include <QCoreApplication>

#ifdef USE_QT_QUICK
#include "qml/src/qmlmanager.hpp"
#endif

int main(int argc, char *argv[])
{
	// Set name for settings etc.
	QCoreApplication::setOrganizationName("kraxarn");
	QCoreApplication::setApplicationName("spotify-qt");
	QCoreApplication::setApplicationVersion(APP_VERSION);

	// Create Qt application
	QApplication app(argc, argv);

	// Settings
	QtPaths paths(nullptr);
	Settings settings(paths);

	// Create QML engine if requested
#ifdef USE_QT_QUICK
	QmlManager qml(settings);
#endif

	// Check fallback icons
	Icon::useFallbackIcons = settings.general.fallbackIcons;

	// Show version if requested
	QCommandLineParser parser;
	parser.addVersionOption();
	parser.addHelpOption();
	parser.addOptions({
		{"debug", "Enable debug menu for troubleshooting issues."}
	});
	parser.process(app);

	if (parser.isSet("debug"))
		MainMenu::showDebugMenu = true;

	// First setup window
	if (settings.account.refreshToken.isEmpty())
	{
#ifdef USE_QT_QUICK
		if (qml.setup())
			return 0;
#else
		SetupDialog dialog(settings);
		if (dialog.exec() == QDialog::Rejected)
			return 0;
#endif
	}

#ifdef USE_QT_QUICK
	qml.main();
#else
	// Create main window
	MainWindow w(settings);

	// Show window and run application
	if (!w.isValid())
		return 1;

	w.show();
#endif

	// Run application
	return QApplication::exec();
}