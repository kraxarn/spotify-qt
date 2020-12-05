#pragma once

#include "../keyring/kwallet.hpp"
#include "../mainwindow.hpp"
#include "../settingspage/aboutpage.hpp"
#include "../settingspage/applicationpage.hpp"
#include "../settingspage/interfacepage.hpp"
#include "../settingspage/playlistspage.hpp"
#include "../settingspage/settingspage.hpp"
#include "../settingspage/spotifypage.hpp"
#include "../spotify/clienthandler.hpp"
#include "../util/utils.hpp"
#include "../view/clienthandlerlogview.hpp"
#include "openlinkdialog.hpp"
#include "settings.hpp"
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
#include <QStackedWidget>
#include <QStyleFactory>
#include <QVBoxLayout>

class SettingsDialog: public QDialog
{
Q_OBJECT

public:
	explicit SettingsDialog(Settings &settings, QWidget *parent = nullptr);

private:
	void categoryChanged(int row);
	bool applySettings();

	Settings &settings;

	QListWidget *categories = nullptr;
	QStackedWidget *stack = nullptr;

	QList<SettingsPage*> pages;
};