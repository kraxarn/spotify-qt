#pragma once

#include "util/icon.hpp"
#include "view/cacheview.hpp"
#include "view/configview.hpp"
#include "view/debugview.hpp"
#include "view/crashes.hpp"
#include "settingspage.hpp"

class AboutPage: public SettingsPage
{
public:
	AboutPage(lib::settings &settings, lib::cache &cache, QWidget *parent);

	auto icon() -> QIcon override;
	auto title() -> QString override;
	auto save() -> bool override;

private:
	auto about() -> QWidget *;
	auto systemInfo() -> QWidget *;
	auto cacheInfo() -> QWidget *;
	auto configPreview() -> QWidget *;
	auto crashLogs(lib::cache &cache) -> QWidget *;

	lib::paths *paths = nullptr;
};
