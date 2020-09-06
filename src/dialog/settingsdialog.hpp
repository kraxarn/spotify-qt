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
	bool applySettings();

	Settings &settings;
	QVector<SettingsPage*> pages;
};