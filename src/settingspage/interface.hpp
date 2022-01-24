#pragma once

#include "settingspage/base.hpp"
#include "util/icon.hpp"

#include <QStyleFactory>
#include <QGroupBox>

namespace SettingsPage
{
	class Interface: public Base
	{
	Q_OBJECT

	public:
		Interface(lib::settings &settings, QWidget *parent);

		auto icon() -> QIcon override;
		auto title() -> QString override;
		auto save() -> bool override;

	private:
		// General
		QComboBox *resizeMode = nullptr;
		QCheckBox *monoTime = nullptr;
		QCheckBox *trackNumbers = nullptr;
		QCheckBox *relativeAdded = nullptr;

		// Appearance
		QComboBox *qtStyle = nullptr;
		QCheckBox *darkTheme = nullptr;
		QCheckBox *iconFallback = nullptr;
		QCheckBox *customFont = nullptr;

		// Tray icon
		QGroupBox *trayEnabled = nullptr;
		QCheckBox *albumInTray = nullptr;
		QCheckBox *invertTrayIcon = nullptr;
		QCheckBox *notifyTrackChange = nullptr;
		QCheckBox *expandAlbumCover = nullptr;

		// Title bar
		QGroupBox *appTitleBar = nullptr;
		QCheckBox *mirrorTitleBar = nullptr;

		auto general() -> QWidget *;
		auto appearance() -> QWidget *;
		auto trayIcon() -> QWidget *;
		auto titleBar() -> QWidget *;

		void saveGeneral();
		void saveAppearance();
		void saveTrayIcon();
		void saveTitleBar();

		static auto hasIconTheme() -> bool;
		static auto defaultStyle() -> QString;
		void darkThemeToggle(bool checked);
	};
}
