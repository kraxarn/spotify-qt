#pragma once

#include "settingspage.hpp"

class AppSettings: QWidget, public SettingsPage
{
Q_OBJECT

public:
	AppSettings(Settings &settings, QWidget *parent);

	static bool isPulse();

private:
	QString title() override;
	QWidget *toWidget() override;
	bool applySettings(QWidget* mainWindow) override;

	QCheckBox *appMedia = nullptr;
	QCheckBox *appPulse = nullptr;
	QCheckBox *appSptOrder = nullptr;
	QCheckBox *appWhatsNew = nullptr;
	QComboBox *appRefresh = nullptr;
	QCheckBox *appOneClick = nullptr;
};