#pragma once

#include "settingspage.hpp"
#include "../util/icon.hpp"

#include <QStyleFactory>

class InterfacePage: public SettingsPage
{
Q_OBJECT

public:
	InterfacePage(lib::settings &settings, QWidget *parent);

	auto icon() -> QIcon override;
	auto title() -> QString override;
	auto save() -> bool override;

private:
	// General
	QCheckBox *itfMonoTime = nullptr;
	QCheckBox *itfRelativeAdded = nullptr;
	QCheckBox *itfTrackNum = nullptr;
	QComboBox *itfResizeMode = nullptr;
	QCheckBox *titleBar = nullptr;

	// Appearance
	QComboBox *itfStyle = nullptr;
	QCheckBox *itfDark = nullptr;
	QCheckBox *itfIcFallback = nullptr;
	QCheckBox *itfFont = nullptr;

	// Tray icon
	QCheckBox *itfTrayAlbum = nullptr;
	QCheckBox *itfTrayIcon = nullptr;
	QCheckBox *itfTrayInvert = nullptr;

	auto general() -> QWidget *;
	auto appearance() -> QWidget *;
	auto trayIcon() -> QWidget *;

	static auto hasIconTheme() -> bool;
	static auto defaultStyle() -> QString;
	void darkThemeToggle(bool checked);
};
