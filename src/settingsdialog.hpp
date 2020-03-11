#pragma once

#include "src/settings.hpp"

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

class SettingsDialog : public QDialog
{
	Q_OBJECT
public:
	explicit SettingsDialog(QWidget *parent = nullptr);
private:
	QGroupBox *appSettings();
	QGroupBox *spotifySettings();
	bool applySettings();
	void applyFail(const QString &setting);
	Settings settings;
	static QString sptClient(const QString &path);
	static bool isPulse();
	// All settings properties
	QComboBox	*appTheme		= nullptr;
	QCheckBox	*appPulse		= nullptr;
	QCheckBox	*appPalette		= nullptr;
	QLineEdit	*sptPath		= nullptr;
	QLabel		*sptVersion		= nullptr;
	QCheckBox	*sptStartClient	= nullptr;
	QLineEdit	*sptUsername	= nullptr;
	QComboBox	*sptBitrate		= nullptr;
};