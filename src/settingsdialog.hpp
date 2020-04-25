#pragma once

#include "settings.hpp"
#include "mainwindow.hpp"

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
	explicit SettingsDialog(QWidget *parent = nullptr);
private:
	QWidget *appSettings();
	QWidget *interfaceSettings();
	QWidget *spotifySettings();
	QWidget *aboutSettings();
	bool applySettings();
	void applyFail(const QString &setting);
	Settings settings;
	static QString sptClient(const QString &path);
	static bool isPulse();
	// All settings properties
	QCheckBox	*appPulse		= nullptr;
	QLineEdit	*sptPath		= nullptr;
	QLabel		*sptVersion		= nullptr;
	QCheckBox	*sptStartClient	= nullptr;
	QLineEdit	*sptUsername	= nullptr;
	QComboBox	*sptBitrate		= nullptr;
	QCheckBox	*appMedia		= nullptr;
	QCheckBox	*darkTheme		= nullptr;
	QCheckBox	*sptOrder		= nullptr;
	QCheckBox	*appTrayIcon	= nullptr;
	QCheckBox	*appTrayNotify	= nullptr;
};