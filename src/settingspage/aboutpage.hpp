#pragma once

#include "settingspage.hpp"
#include "../util/icon.hpp"

class AboutPage: public SettingsPage
{
public:
	AboutPage(Settings &settings, QWidget *parent);

	QIcon icon() override;
	QString title() override;
	bool save() override;

private:
	QWidget *about();
	QWidget *systemInfo();
	QWidget *appLogs();
};
