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
		QCheckBox *trackNumbers = nullptr;
		QCheckBox *relativeAdded = nullptr;
		QCheckBox *nativeWindow = nullptr;

		// Appearance
		QComboBox *qtStyle = nullptr;
		QCheckBox *darkTheme = nullptr;
		QCheckBox *iconFallback = nullptr;
		QLabel *currentFont = nullptr;

		QString fontName;
		int fontSize = 0;

		// Tray icon
		QGroupBox *trayEnabled = nullptr;
		QCheckBox *albumInTray = nullptr;
		QCheckBox *invertTrayIcon = nullptr;
		QCheckBox *notifyTrackChange = nullptr;
		QCheckBox *expandAlbumCover = nullptr;
		QCheckBox *closeToTray = nullptr;

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

		void onSelectFont(bool checked);
		void onResetFont(bool checked);

		static auto hasIconTheme() -> bool;
		static auto defaultStyle() -> QString;
		void darkThemeToggle(bool checked);

		static auto getFontName(const QString &family, int pointSize) -> QString;
		static auto getFontName(const QFont &font) -> QString;
		static auto getDefaultFontName() -> QString;
		static auto getDefaultFont() -> QFont;
	};
}
