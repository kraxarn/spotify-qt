#pragma once

#include "lib/cache.hpp"
#include "lib/crash/crashhandler.hpp"

#include "util/icon.hpp"
#include "lib/qtpaths.hpp"
#include "settingspage/settingspage.hpp"
#include "view/configview.hpp"
#include "view/cacheview.hpp"
#include "view/crashes.hpp"
#include "view/systeminfoview.hpp"

class TroubleshootPage: public SettingsPage
{
public:
	TroubleshootPage(lib::settings &settings, lib::cache &cache, QWidget *parent);

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
