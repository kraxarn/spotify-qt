#pragma once

#include "../util/icon.hpp"
#include "lib/settings.hpp"
#include "settingspage.hpp"

#include <QLabel>
#include <QComboBox>

class ApplicationPage: public SettingsPage
{
public:
	ApplicationPage(lib::settings &settings, QWidget *parent);

	QIcon icon() override;
	QString title() override;
	bool save() override;

private:
	QCheckBox *appMedia = nullptr;
	QCheckBox *appPulse = nullptr;
	QCheckBox *appSptOrder = nullptr;
	QCheckBox *appWhatsNew = nullptr;
	QComboBox *appRefresh = nullptr;
	QComboBox *appMaxQueue = nullptr;

	static bool isPulse();

	QWidget *app();
	QWidget *appLogs();
};


