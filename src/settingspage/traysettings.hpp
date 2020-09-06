#pragma once

#include "settingspage.hpp"

class TraySettings: QWidget, public SettingsPage
{
Q_OBJECT

public:
	TraySettings(Settings &settings, QWidget *parent);

	QString title() override;
	QWidget *toWidget() override;
	bool applySettings(QWidget *mainWindow) override;

private:
	QCheckBox *itfTrayAlbum = nullptr;
	QCheckBox *itfTrayIcon = nullptr;
	QCheckBox *itfTrayInvert = nullptr;
	QCheckBox *itfTrayNotify = nullptr;
};