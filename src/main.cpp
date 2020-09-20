#include "dialog/setupdialog.hpp"
#include "mainwindow.hpp"
#include "util/icon.hpp"

#include <QApplication>
#include <QCoreApplication>

int main(int argc, char *argv[])
{
	// Set name for settings etc.
	QCoreApplication::setOrganizationName("kraxarn");
	QCoreApplication::setApplicationName("spotify-qt");
	QCoreApplication::setApplicationVersion(APP_VERSION);

	// Create Qt application
	QApplication app(argc, argv);

	// JSON Settings
	Settings settings;
	if (!QFile::exists(Settings::fileName()))
	{
		qDebug() << "no json settings, attempting to convert legacy settings...";
		QDir::root().mkpath(Settings::filePath());
		QFile jsonFile(Settings::fileName());
		jsonFile.open(QIODevice::WriteOnly);
		auto jsonData = settings.legacyToJson().toJson();
		jsonFile.write(jsonData);
		jsonFile.close();
		settings.load();
	}

	// Check fallback icons
	Icon::useFallbackIcons = settings.general.fallbackIcons;

	// Show version if requested
	QCommandLineParser parser;
	parser.addVersionOption();
	parser.process(app);

	// First setup window
	if (settings.account.refreshToken.isEmpty())
	{
		SetupDialog dialog(settings);
		if (dialog.exec() == QDialog::Rejected)
			return 0;
	}

	// Create main window
	MainWindow w(settings);

	// Show window and run application
	w.show();
	return QApplication::exec();
}