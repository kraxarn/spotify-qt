#pragma once

#include "../mainwindow.hpp"
#include "../settings.hpp"
#include "../spotify/clienthandler.hpp"
#include "clienthandlerlogdialog.hpp"
#include "openlinkdialog.hpp"
#include "systeminfodialog.hpp"
#include "../settingspage/settingspage.hpp"
#include "../settingspage/pages.hpp"

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

class SettingsDialog: public QDialog
{
Q_OBJECT

public:
	explicit SettingsDialog(Settings &settings, QWidget *parent = nullptr);

private:
	QWidget *interfaceSettings();
	QWidget *traySettings();
	QWidget *spotifySettings();
	QWidget *aboutSettings();
	bool applySettings();
	static bool sptConfigExists();
	void globalConfigToggle(int state);

	Settings &settings;
	QVector<SettingsPage> pages;

	// Tray icon settings
	QCheckBox *itfTrayAlbum = nullptr;
	QCheckBox *itfTrayIcon = nullptr;
	QCheckBox *itfTrayInvert = nullptr;
	QCheckBox *itfTrayNotify = nullptr;

	// Spotify settings
	QCheckBox *sptAlways = nullptr;
	QCheckBox *sptAppStart = nullptr;
	QCheckBox *sptGlobal = nullptr;
	QComboBox *sptBitrate = nullptr;
	QGroupBox *sptGroup = nullptr;
	QLabel *sptVersion = nullptr;
	QLineEdit *sptPath = nullptr;
	QLineEdit *sptUsername = nullptr;
};