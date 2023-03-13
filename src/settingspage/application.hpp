#pragma once

#include "util/icon.hpp"
#include "lib/settings.hpp"
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

	private:
		QCheckBox *appMedia = nullptr;
		QCheckBox *appHotkeys = nullptr;
		QCheckBox *appWhatsNew = nullptr;
		QComboBox *appRefresh = nullptr;
		QComboBox *appMaxQueue = nullptr;
		QCheckBox *appUpdates = nullptr;

		static constexpr int minRefreshInterval = 1;
		static constexpr int maxRefreshInterval = 60;

		static constexpr int minMaxQueue = 1;
		static constexpr int maxMaxQueue = 1000;

		auto app() -> QWidget *;
	};
}
