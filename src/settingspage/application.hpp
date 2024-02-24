#pragma once

#include "util/icon.hpp"
#include "lib/settings.hpp"
#include "lib/spotify/playback.hpp"
#include "settingspage/base.hpp"
#include "util/widget.hpp"

namespace SettingsPage
{
	class Application: public Base
	{
	public:
		Application(lib::settings &settings, QWidget *parent);

		auto icon() -> QIcon override;
		auto title() -> QString override;
		auto save() -> bool override;

	protected:
		void hideEvent(QHideEvent *event) override;
		void showEvent(QShowEvent *event) override;

	private:
		QCheckBox *appMedia = nullptr;
		QCheckBox *appHotkeys = nullptr;
		QCheckBox *appWhatsNew = nullptr;
		QComboBox *appRefresh = nullptr;
		QComboBox *appMaxQueue = nullptr;
		QCheckBox *appUpdates = nullptr;
		QCheckBox *ignoreUnavailableIndex = nullptr;

		QLineEdit *titleFormat = nullptr;
		QLabel *titlePreview = nullptr;

		lib::spt::track currentTrack;

		static constexpr int minRefreshInterval = 1;
		static constexpr int maxRefreshInterval = 60;

		static constexpr int minMaxQueue = 1;
		static constexpr int maxMaxQueue = 1000;

		auto app() -> QWidget *;
		auto windowTitle() -> QWidget *;

		void updatePreview();

		auto saveGeneral() -> bool;
		auto saveTitle() const -> bool;

		void onPlaybackRefreshed(const lib::spt::playback &current,
			const lib::spt::playback &previous);

		void onTitleFormatEdited(const QString &text);
	};
}
