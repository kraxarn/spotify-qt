#pragma once

#include "keyring/kwallet.hpp"
#include "mainwindow.hpp"
#include "settingspage/about.hpp"
#include "settingspage/application.hpp"
#include "settingspage/interface.hpp"
#include "settingspage/playlists.hpp"
#include "settingspage/base.hpp"
#include "settingspage/spotify.hpp"
#include "settingspage/troubleshoot.hpp"
#include "spotifyclient/runner.hpp"
#include "view/clienthandlerlogview.hpp"
#include "view/systeminfoview.hpp"
#include "lib/settings.hpp"
#include "dialog/openlink.hpp"

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
#include <QListWidget>
#include <QMessageBox>
#include <QProcess>
#include <QPushButton>
#include <QStackedWidget>
#include <QStyleFactory>
#include <QVBoxLayout>

class SettingsDialog: public QDialog
{
Q_OBJECT

public:
	SettingsDialog(lib::settings &settings, lib::cache &cache,
		const lib::http_client &httpClient, QWidget *parent);

private:
	void categoryChanged(int row);
	auto applySettings() -> bool;

	lib::settings &settings;

	QListWidget *categories = nullptr;
	QStackedWidget *stack = nullptr;

	QList<SettingsPage::Base *> pages;
};
