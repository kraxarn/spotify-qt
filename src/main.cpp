#include "mainwindow.h"

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
	// TODO
	// Create main window
	MainWindow w;
	// Show window and run application
	w.show();
	return QApplication::exec();
}