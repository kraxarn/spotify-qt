#pragma once

#include "mainwindow.hpp"
#include "settings.hpp"
#include "spotify/clienthandler.hpp"
#include "systeminfodialog.hpp"

#include <QAction>
#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QComboBox>
#include <QDebug>
#include <QDialog>
#include <QFileDialog>
#include <QFileInfo>
#include <QFileInfo>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QProcess>
#include <QPushButton>
#include <QStringRef>
#include <QStyleFactory>
#include <QVBoxLayout>

class SettingsDialog : public QDialog
{
	Q_OBJECT

public:
	explicit SettingsDialog(Settings &settings, QWidget *parent = nullptr);

private:
	QWidget *appSettings();
	QWidget *interfaceSettings();
	QWidget *traySettings();
	QWidget *spotifySettings();
	QWidget *aboutSettings();
	bool applySettings();
	void applyFail(const QString &setting);
	static bool isPulse();
	static bool sptConfigExists();
	static bool hasIconTheme();
	static QString systemInfo();
	Settings &settings;
	// App settings
	QComboBox	*appRefresh		= nullptr;
	QCheckBox	*appPulse		= nullptr;
	QCheckBox	*appMedia		= nullptr;
	QCheckBox	*appSptOrder	= nullptr;
	QCheckBox	*appWhatsNew	= nullptr;
	// Interface settings
	QCheckBox	*itfDark		= nullptr;
	QCheckBox	*itfResizeAuto	= nullptr;
	QCheckBox	*itfIcFallback	= nullptr;
	QCheckBox	*itfMonoTime	= nullptr;
	// Tray icon settings
	QCheckBox	*itfTrayIcon	= nullptr;
	QCheckBox	*itfTrayNotify	= nullptr;
	QCheckBox	*itfTrayInvert	= nullptr;
	QCheckBox	*itfTrayAlbum	= nullptr;
	// Spotify settings
	QLineEdit	*sptPath		= nullptr;
	QLabel		*sptVersion		= nullptr;
	QLineEdit	*sptUsername	= nullptr;
	QComboBox	*sptBitrate		= nullptr;
	QCheckBox	*sptAppStart	= nullptr;
	QCheckBox	*sptGlobal		= nullptr;
};