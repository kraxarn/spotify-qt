#include "dialog/setupdialog.hpp"
#include "mainwindow.hpp"
#include "util/icon.hpp"

#include <QApplication>
#include <QCoreApplication>

#ifdef USE_QT_QUICK

#include "../qml/src/spotifyqml.hpp"
#include "../qml/src/utilsqml.hpp"
#include "../qml/src/settingsqml.hpp"

#include <QQmlApplicationEngine>
#include <QQuickStyle>
#include <QtWebEngine>

#endif

#ifdef USE_QT_QUICK
void defineTypes(QQmlApplicationEngine &engine)
{
	engine.rootContext()->setContextProperty("AppVersion", APP_VERSION);
	engine.rootContext()->setContextProperty(
		"QtVersion",
		QString("%1.%2").arg(QT_VERSION_MAJOR).arg(QT_VERSION_MINOR));

	qmlRegisterType<SpotifyQml>(
		"com.kraxarn.spotify",
		1, 0,
		"Spotify");

	qmlRegisterType<UtilsQml>(
		"com.kraxarn.utils",
		1, 0,
		"Utils");

	qmlRegisterType<SettingsQml>(
		"com.kraxarn.settings",
		1, 0,
		"Settings");
}
#endif

int main(int argc, char *argv[])
{
	// Set name for settings etc.
	QCoreApplication::setOrganizationName("kraxarn");
	QCoreApplication::setApplicationName("spotify-qt");
	QCoreApplication::setApplicationVersion(APP_VERSION);

#ifdef USE_QT_QUICK
	/*
	 * Web engine is probably needed on mobile devices
	 * for authentication as a temporary web server
	 * probably can't be created
	 */
	QtWebEngine::initialize();
#endif

	// Create Qt application
	QApplication app(argc, argv);
#ifdef USE_QT_QUICK
	QQmlApplicationEngine engine;
	defineTypes(engine);
#endif

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
#ifdef USE_QT_QUICK
		engine.load(QUrl("qrc:/setup.qml"));
		QApplication::exec();
		settings.load();
		if (settings.account.accessToken.isEmpty() || settings.account.refreshToken.isEmpty())
			return 0;
#else
		SetupDialog dialog(settings);
		if (dialog.exec() == QDialog::Rejected)
			return 0;
#endif
	}

#ifdef USE_QT_QUICK
	QQuickStyle::setStyle(QQuickStyle::availableStyles().contains(settings.general.style)
		? settings.general.style
		: "Material");
	qDebug() << "using" << QQuickStyle::name() << "style";
	QQmlApplicationEngine::connect(
		&engine, &QQmlApplicationEngine::objectCreated, &app,
		[](QObject *obj, const QUrl &url)
		{
			if (obj == nullptr)
				QCoreApplication::quit();
		}, Qt::QueuedConnection);
	engine.load(QUrl("qrc:/main.qml"));
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