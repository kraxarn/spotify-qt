#include "mainwindow.hpp"
#include "setupdialog.hpp"

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
	// Create Qt application
	QApplication app(argc, argv);
#ifdef USE_CRASH_HANDLER
	// We want to enable KDE crash handler
	KCrash::initialize();
	if (!KCrash::isDrKonqiEnabled())
		qDebug() << "warning: crash handler enabled, but dr konqi is not";
	qDebug() << "crash handler initialized";
#endif
	// First setup window
	if (Settings().refreshToken().isEmpty())
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