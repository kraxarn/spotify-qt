#pragma once

#include "settingspage.hpp"
#include "../util/icon.hpp"

class InterfacePage: public SettingsPage
{
public:
	InterfacePage(Settings &settings, QWidget *parent);

	QIcon icon() override;
	QString title() override;
	bool save() override;

private:
	QCheckBox *itfContextInfo = nullptr;
	QCheckBox *itfDark = nullptr;
	QCheckBox *itfIcFallback = nullptr;
	QCheckBox *itfMonoTime = nullptr;
	QCheckBox *itfResizeAuto = nullptr;
	QCheckBox *itfTrackNum = nullptr;

	static bool hasIconTheme();
};


