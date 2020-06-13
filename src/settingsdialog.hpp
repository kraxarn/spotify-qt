#pragma once

#include "settings.hpp"
#include "mainwindow.hpp"
#include "spotify/clienthandler.hpp"

#include <QDialog>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QCheckBox>
#include <QComboBox>
#include <QApplication>
#include <QStyleFactory>
#include <QDebug>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QProcess>
#include <QStringRef>
#include <QAction>
#include <QFileInfo>
#include <QInputDialog>
#include <QMessageBox>
#include <QFileInfo>
#include <QFileDialog>

class SettingsDialog : public QDialog
{
	Q_OBJECT

public:
	explicit SettingsDialog(Settings &settings, QWidget *parent = nullptr);

private:
	QWidget *appSettings();
	QWidget *interfaceSettings();
	QWidget *spotifySettings();
	QWidget *aboutSettings();
	bool applySettings();
	void applyFail(const QString &setting);
	static bool isPulse();
	static bool sptConfigExists();
	static bool hasIconTheme();
	Settings &settings;
	// App settings
	QComboBox	*appRefresh		= nullptr;
	QCheckBox	*appPulse		= nullptr;
	QCheckBox	*appMedia		= nullptr;
	QCheckBox	*appSptOrder	= nullptr;
	QCheckBox	*appWhatsNew	= nullptr;
	// Interface settings
	QCheckBox	*itfDark		= nullptr;
	QCheckBox	*itfTrayIcon	= nullptr;
	QCheckBox	*itfTrayNotify	= nullptr;
	QCheckBox	*itfTrayInvert	= nullptr;
	QCheckBox	*itfResizeAuto	= nullptr;
	QCheckBox	*itfIcFallback	= nullptr;
	QCheckBox	*itfMonoTime	= nullptr;
	QCheckBox	*itfTrayAlbum	= nullptr;
	// Spotify settings
	QLineEdit	*sptPath		= nullptr;
	QLabel		*sptVersion		= nullptr;
	QLineEdit	*sptUsername	= nullptr;
	QComboBox	*sptBitrate		= nullptr;
	QCheckBox	*sptAppStart	= nullptr;
	QCheckBox	*sptGlobal		= nullptr;
};