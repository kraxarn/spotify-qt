#include "mainwindow.hpp"
#include "setupdialog.hpp"
#include "src/spotify/spotify.hpp"

#include <QApplication>
#include <QCoreApplication>
#include <QDesktopServices>

int main(int argc, char *argv[])
{
	// Set name for settings etc.
	QCoreApplication::setOrganizationName("kraxarn");
	QCoreApplication::setApplicationName("spotify-qt");
	// Create Qt application
	QApplication app(argc, argv);
	// First setup window
	if (Settings().refreshToken().isEmpty())
	{
		SetupDialog dialog;
		if (dialog.exec() == QDialog::Rejected)
			return 0;
	}
	// Spotify class instance
	spt::Spotify spotify;
	// Create main window
	MainWindow w(&spotify, &app);
	// Show window and run application
	w.show();
	return QApplication::exec();
}