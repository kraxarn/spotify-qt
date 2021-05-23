#pragma once

#include "../util/icon.hpp"
#include "lib/settings.hpp"
#include "settingspage.hpp"
#include "view/logview.hpp"

class ApplicationPage: public SettingsPage
{
public:
	ApplicationPage(lib::settings &settings, QWidget *parent);

	auto icon() -> QIcon override;
	auto title() -> QString override;
	auto save() -> bool override;

private:
	QCheckBox *appMedia = nullptr;
	QCheckBox *appPulse = nullptr;
	QCheckBox *appWhatsNew = nullptr;
	QComboBox *appRefresh = nullptr;
	QComboBox *appMaxQueue = nullptr;

	static bool isPulse();

	static auto isPulse() -> bool;

	auto app() -> QWidget *;
	auto appLogs() -> QWidget *;
};


