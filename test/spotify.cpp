#include "tests.hpp"

class SpotifyTests: public QObject
{
Q_OBJECT

private:
	spt::Spotify *spotify = nullptr;
	Settings settings;

private slots:
	static void initTestCase()
	{
		QCoreApplication::setOrganizationName("kraxarn");
		QCoreApplication::setApplicationName("spotify-qt");
	}

	static void configExists()
	{
		QFileInfo filePath(
			QString("%1.json")
				.arg(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation)));

		QVERIFY(filePath.exists());
	}

	void testAuthentication()
	{
		spotify = new spt::Spotify(settings, this);
	}

	void cleanupTestCase()
	{
		delete spotify;
	}
};

QTEST_MAIN(SpotifyTests)
#include "spotify.moc"