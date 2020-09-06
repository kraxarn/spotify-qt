#pragma once

#include "settingspage.hpp"

class AboutSettings: QWidget, public SettingsPage
{
Q_OBJECT

public:
	AboutSettings(Settings &settings, QWidget *parent);

	QString title() override;
	QWidget *toWidget() override;
	bool applySettings(QWidget *mainWindow) override;
};