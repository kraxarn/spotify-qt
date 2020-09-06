#pragma once

#include "settingspage.hpp"

class InterfaceSettings: public SettingsPage
{
public:
	InterfaceSettings(Settings &settings, QWidget *parent);

private:
	QString title() override;
	bool applySettings(QWidget* mainWindow) override;

	static bool hasIconTheme();

	QCheckBox *itfContextInfo = nullptr;
	QCheckBox *itfDark = nullptr;
	QCheckBox *itfIcFallback = nullptr;
	QCheckBox *itfMonoTime = nullptr;
	QCheckBox *itfResizeAuto = nullptr;
};


