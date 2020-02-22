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
#include <QMessageBox>

class SettingsDialog : public QDialog
{
	Q_OBJECT
public:
	explicit SettingsDialog(QWidget *parent = nullptr);
private:
	QGroupBox *appSettings();
	QGroupBox *spotifySettings();
	QStringList backends();
	bool applySettings();
	void applyFail(const QString &setting);
	Settings settings;
	QString sptClient(const QString &path);
	QString clientVersion(const QFileInfo &fileInfo);
	// All settings properties
	QComboBox	*appTheme	= nullptr;
	QLineEdit	*sptPath	= nullptr;
	QLabel		*sptVersion	= nullptr;
};