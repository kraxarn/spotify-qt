#pragma once

#include "settingspage.hpp"

class TraySettings: public SettingsPage
{
public:
	TraySettings(Settings &settings, QWidget *parent);

	QString title() override;
	bool applySettings(QWidget *mainWindow) override;

private:
	QCheckBox *itfTrayAlbum = nullptr;
	QCheckBox *itfTrayIcon = nullptr;
	QCheckBox *itfTrayInvert = nullptr;
	QCheckBox *itfTrayNotify = nullptr;
};