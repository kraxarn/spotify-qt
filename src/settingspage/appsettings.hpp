#pragma once

#include "settingspage.hpp"

class AppSettings: public SettingsPage
{
public:
	AppSettings(Settings &settings, QWidget *parent);

	static bool isPulse();

private:
	QString title() override;
	bool applySettings(QWidget* mainWindow) override;

	QCheckBox *appMedia = nullptr;
	QCheckBox *appPulse = nullptr;
	QCheckBox *appSptOrder = nullptr;
	QCheckBox *appWhatsNew = nullptr;
	QComboBox *appRefresh = nullptr;
	QCheckBox *appOneClick = nullptr;
};