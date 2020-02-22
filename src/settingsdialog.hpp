#pragma once

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

class SettingsDialog : public QDialog
{
	Q_OBJECT
public:
	explicit SettingsDialog(QWidget *parent = nullptr);
private:
	QGroupBox *appSettings();
	QGroupBox *spotifydSettings();
	QStringList backends();
	void applySettings();
	// All settings properties
	QComboBox	*appTheme;
};