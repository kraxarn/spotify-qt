#pragma once

#include "lib/cache.hpp"
#include "lib/httpclient.hpp"
#include "lib/spotify/callback.hpp"
#include "settingspage/base.hpp"
#include "util/icon.hpp"
#include "widget/albumshapecombobox.hpp"

#include <QGroupBox>
#include <QStyleFactory>

namespace SettingsPage
{
	class Interface: public Base
	{
	Q_OBJECT

	public:
		Interface(lib::settings &settings, const HttpClient &httpClient,
			lib::cache &cache, QWidget *parent);

		auto icon() -> QIcon override;
		auto title() -> QString override;
		auto save() -> bool override;

	private:
		const HttpClient &httpClient;
		lib::cache &cache;

		// General
		QComboBox *resizeMode = nullptr;
		QComboBox *toolbarPosition = nullptr;
		AlbumShapeComboBox *albumShape = nullptr;
		QCheckBox *trackNumbers = nullptr;
		QCheckBox *relativeAdded = nullptr;
		QCheckBox *tabbedLibrary = nullptr;

		// Appearance
		QComboBox *qtStyle = nullptr;
		QCheckBox *darkTheme = nullptr;
		QCheckBox *iconFallback = nullptr;
		QLabel *currentFont = nullptr;

		QString fontName;
		int fontSize = 0;

		// Tray icon
		QGroupBox *trayEnabled = nullptr;
		QCheckBox *notifyTrackChange = nullptr;
		QCheckBox *expandAlbumCover = nullptr;
		QCheckBox *closeToTray = nullptr;
		QComboBox *trayIconType = nullptr;

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

		static auto albumShapes() -> QList<lib::album_shape>;

		static auto getFontName(const QString &family, int pointSize) -> QString;
		static auto getFontName(const QFont &font) -> QString;
		static auto getDefaultFontName() -> QString;
		static auto getDefaultFont() -> QFont;
		void albumArtIcon(const ApiCallback<QIcon> &callback) const;
	};
}
