#pragma once

#include "../util/utils.hpp"
#include "../settings.hpp"
#include "../mainwindow.hpp"

#include <QVBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QCheckBox>
#include <QMessageBox>

class SettingsPage
{
public:
	virtual QString title() = 0;
	virtual bool applySettings(QWidget* mainWindow) = 0;

	QWidget *toWidget();

protected:
	SettingsPage(Settings &settings, QWidget *parent);

	void applyFail(const QString &name);

	QVBoxLayout *page = nullptr;
	QWidget *parent = nullptr;
	Settings &settings;
};