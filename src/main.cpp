#include "mainwindow.h"
#include "src/spotify/spotify.hpp"

#include <QApplication>
#include <QCoreApplication>

int main(int argc, char *argv[])
{
	// Set name for settings etc.
	QCoreApplication::setOrganizationName("kraxarn");
	QCoreApplication::setApplicationName("spotify-qt");
	// Create Qt application
	QApplication a(argc, argv);
	// Spotify auth
	spt::Spotify spotify;
	if (!spotify.auth())
		qFatal("error: authentication failed");
	// Create main window
	MainWindow w;
	// Show window and run application
	w.show();
	return QApplication::exec();
}