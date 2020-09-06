#pragma once

#include "settingspage.hpp"

class AboutSettings: public SettingsPage
{
public:
	AboutSettings(Settings &settings, QWidget *parent);

	QString title() override;
	bool applySettings(QWidget *mainWindow) override;
};