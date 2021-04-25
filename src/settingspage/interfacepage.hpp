#pragma once

#include "settingspage.hpp"
#include "../util/icon.hpp"

#include <QStyleFactory>

class InterfacePage: public SettingsPage
{
public:
	InterfacePage(lib::settings &settings, QWidget *parent);

	auto icon() -> QIcon override;
	auto title() -> QString override;
	auto save() -> bool override;

private:
	// General
	QCheckBox *itfContextInfo = nullptr;
	QCheckBox *itfDark = nullptr;
	QCheckBox *itfIcFallback = nullptr;
	QCheckBox *itfMonoTime = nullptr;
	QCheckBox *itfRelativeAdded = nullptr;
	QCheckBox *itfTrackNum = nullptr;
	QComboBox *itfStyle = nullptr;
	QComboBox *itfResizeMode = nullptr;

	// Tray icon
	QCheckBox *itfTrayAlbum = nullptr;
	QCheckBox *itfTrayIcon = nullptr;
	QCheckBox *itfTrayInvert = nullptr;
	QCheckBox *itfTrayNotify = nullptr;

	auto general() -> QWidget *;
	auto trayIcon() -> QWidget *;

	static auto hasIconTheme() -> bool;
	static auto defaultStyle() -> QString;
	void darkThemeToggle(bool checked);
};
