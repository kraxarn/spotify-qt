#pragma once

#include "settingspage.hpp"

class InterfaceSettings: QWidget, public SettingsPage
{
Q_OBJECT

public:
	InterfaceSettings(Settings &settings, QWidget *parent);

private:
	QString title() override;
	QWidget *toWidget() override;
	bool applySettings(QWidget* mainWindow) override;

	static bool hasIconTheme();

	QCheckBox *itfContextInfo = nullptr;
	QCheckBox *itfDark = nullptr;
	QCheckBox *itfIcFallback = nullptr;
	QCheckBox *itfMonoTime = nullptr;
	QCheckBox *itfResizeAuto = nullptr;
};


