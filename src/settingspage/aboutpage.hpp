#pragma once

#include "../appversion.hpp"
#include "../util/icon.hpp"
#include "../view/cacheview.hpp"
#include "settingspage.hpp"

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
	QWidget *cacheInfo();
};
