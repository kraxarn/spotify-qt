#pragma once

#include "../appversion.hpp"
#include "../util/icon.hpp"
#include "../view/cacheview.hpp"
#include "../view/configview.hpp"
#include "../view/debugview.hpp"
#include "settingspage.hpp"

class AboutPage: public SettingsPage
{
public:
	AboutPage(lib::settings &settings, QWidget *parent);

	QIcon icon() override;
	QString title() override;
	bool save() override;

private:
	QWidget *about();
	QWidget *systemInfo();
	QWidget *cacheInfo();
	QWidget *configPreview();
};
