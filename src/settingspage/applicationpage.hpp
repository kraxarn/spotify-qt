#pragma once

#include "settingspage.hpp"
#include "../settings.hpp"
#include "../util/icon.hpp"

#include <QLabel>
#include <QComboBox>

class ApplicationPage: public SettingsPage
{
public:
	ApplicationPage(Settings &settings, QWidget *parent);

	QIcon icon() override;
	QString title() override;
	bool save() override;

private:
	QCheckBox *appMedia = nullptr;
	QCheckBox *appPulse = nullptr;
	QCheckBox *appSptOrder = nullptr;
	QCheckBox *appWhatsNew = nullptr;
	QComboBox *appRefresh = nullptr;
	QCheckBox *appOneClick = nullptr;

	static bool isPulse();

	QWidget *app();
};


