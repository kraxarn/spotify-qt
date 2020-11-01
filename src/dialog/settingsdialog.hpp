#pragma once

#include "../keyring/kwallet.hpp"
#include "../mainwindow.hpp"
#include "../settings.hpp"
#include "../spotify/clienthandler.hpp"
#include "clienthandlerlogdialog.hpp"
#include "openlinkdialog.hpp"
#include "systeminfodialog.hpp"

#include <QAction>
#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QDialog>
#include <QFileDialog>
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
#include <QStyleFactory>
#include <QVBoxLayout>

class SettingsDialog: public QDialog
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
	QWidget *playlistSettings();
	bool applySettings();
	void applyFail(const QString &setting);
	static bool isPulse();
	static bool sptConfigExists();
	static bool hasIconTheme();
	void globalConfigToggle(int state);
	void startClientToggle(int state);
	void playlistOrderChanged(int index);
	void playlistMove(int steps);
	void playlistUp();
	void playlistDown();
	void playlistItemChanged(int row);
	void warning(const QString &title, const QString &message);

	Settings &settings;

	// App settings
	QCheckBox *appMedia = nullptr;
	QCheckBox *appPulse = nullptr;
	QCheckBox *appSptOrder = nullptr;
	QCheckBox *appWhatsNew = nullptr;
	QComboBox *appRefresh = nullptr;
	QCheckBox *appOneClick = nullptr;

	// Interface settings
	QCheckBox *itfContextInfo = nullptr;
	QCheckBox *itfDark = nullptr;
	QCheckBox *itfIcFallback = nullptr;
	QCheckBox *itfMonoTime = nullptr;
	QCheckBox *itfResizeAuto = nullptr;
	QCheckBox *itfTrackNum = nullptr;

	// Tray icon settings
	QCheckBox *itfTrayAlbum = nullptr;
	QCheckBox *itfTrayIcon = nullptr;
	QCheckBox *itfTrayInvert = nullptr;
	QCheckBox *itfTrayNotify = nullptr;

	// Spotify settings
	QCheckBox *sptAlways = nullptr;
	QCheckBox *sptAppStart = nullptr;
	QCheckBox *sptGlobal = nullptr;
	QCheckBox *sptKeyring = nullptr;
	QComboBox *sptBitrate = nullptr;
	QGroupBox *sptGroup = nullptr;
	QLabel *sptVersion = nullptr;
	QLineEdit *sptPath = nullptr;
	QLineEdit *sptUsername = nullptr;

	// Playlist settings
	QComboBox *plOrder = nullptr;
	QHBoxLayout *plListLayout = nullptr;
	QLabel *plHint = nullptr;
	QListWidget *plList = nullptr;
	QStringList plHints;
	QAction *plBtnUp = nullptr;
	QAction *plBtnDown = nullptr;
};