#pragma once

#include "lib/cache.hpp"
#include "lib/crash/crashhandler.hpp"

#include "util/icon.hpp"
#include "lib/qtpaths.hpp"
#include "settingspage/base.hpp"
#include "view/configview.hpp"
#include "view/cacheview.hpp"
#include "view/crashes.hpp"
#include "view/systeminfoview.hpp"

namespace SettingsPage
{
	class Troubleshoot: public Base
	{
	public:
		Troubleshoot(lib::settings &settings, lib::cache &cache, QWidget *parent);

		auto icon() -> QIcon override;
		auto title() -> QString override;
		auto save() -> bool override;

	private:
		lib::cache &cache;
		lib::paths *paths = nullptr;

		auto systemInfo() -> QWidget *;
		auto configPreview() -> QWidget *;
		auto cacheInfo() -> QWidget *;
		auto crashLogs() -> QWidget *;
	};
}
