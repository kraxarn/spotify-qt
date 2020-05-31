#include "mainwindow.hpp"
#include "setupdialog.hpp"
#include "icon.hpp"

#include <QApplication>
#include <QCoreApplication>

#ifdef USE_CRASH_HANDLER
#include <kcrash.h>
#endif

int main(int argc, char *argv[])
{
	// Set name for settings etc.
	QCoreApplication::setOrganizationName("kraxarn");
	QCoreApplication::setApplicationName("spotify-qt");
	QCoreApplication::setApplicationVersion(APP_VERSION);
	// Create Qt application
	QApplication app(argc, argv);
	// JSON Settings
	if (!QFile::exists(Settings::fileName()))
	{
		qDebug() << "no json settings, attempting to convert legacy settings...";
		QFile jsonFile(Settings::fileName());
		jsonFile.open(QIODevice::WriteOnly);
		auto jsonData = Settings::get().legacyToJson().toJson();
		jsonFile.write(jsonData);
		jsonFile.close();
	}
	// Check fallback icons
	auto settings = Settings::get();
	Icon::useFallbackIcons = settings.general.fallbackIcons;
	// Show version if requested
	QCommandLineParser parser;
	parser.addVersionOption();
	parser.process(app);
#ifdef USE_CRASH_HANDLER
	// We want to enable KDE crash handler
	KCrash::initialize();
	if (!KCrash::isDrKonqiEnabled())
		qDebug() << "warning: crash handler enabled, but dr konqi is not";
	qDebug() << "crash handler initialized";
#endif
	// First setup window
	if (settings.account.refreshToken.isEmpty())
	{
		SetupDialog dialog;
		if (dialog.exec() == QDialog::Rejected)
			return 0;
	}
	// Create main window
	MainWindow w;
	// Show window and run application
	w.show();
	return QApplication::exec();
}