#pragma once

#include "settingspage.hpp"
#include "../util/icon.hpp"

class InterfacePage: public SettingsPage
{
public:
	InterfacePage(lib::settings &settings, QWidget *parent);

	QIcon icon() override;
	QString title() override;
	bool save() override;

private:
	// General
	QCheckBox *itfContextInfo = nullptr;
	QCheckBox *itfDark = nullptr;
	QCheckBox *itfIcFallback = nullptr;
	QCheckBox *itfMonoTime = nullptr;
	QCheckBox *itfRelativeAdded = nullptr;
	QCheckBox *itfResizeAuto = nullptr;
	QCheckBox *itfTrackNum = nullptr;
	QComboBox *itfStyle = nullptr;

	// Tray icon
	QCheckBox *itfTrayAlbum = nullptr;
	QCheckBox *itfTrayIcon = nullptr;
	QCheckBox *itfTrayInvert = nullptr;
	QCheckBox *itfTrayNotify = nullptr;

	QWidget *general();
	QWidget *trayIcon();

	static bool hasIconTheme();
	void darkThemeToggle(bool checked);
};


