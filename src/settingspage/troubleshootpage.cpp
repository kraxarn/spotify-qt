#include "troubleshootpage.hpp"

TroubleshootPage::TroubleshootPage(lib::settings &settings, lib::cache &cache, QWidget *parent)
	: SettingsPage(settings, parent),
	cache(cache)
{
	addTab(systemInfo(), "System information");
	addTab(configPreview(), "Config preview");
	addTab(cacheInfo(), "Cache");

	if (lib::crash_handler::is_init()
		|| !cache.get_all_crashes().empty())
	{
		addTab(crashLogs(), "Crashes");
	}
}

auto TroubleshootPage::systemInfo() -> QWidget *
{
	return new SystemInfoView(this);
}

auto TroubleshootPage::configPreview() -> QWidget *
{
	return new ConfigView(settings, this);
}

auto TroubleshootPage::cacheInfo() -> QWidget *
{
	if (paths == nullptr)
	{
		paths = new QtPaths(this);
	}
	return new CacheView(*paths, this);
}

auto TroubleshootPage::crashLogs() -> QWidget *
{
	return new View::Crashes(cache, this);
}

auto TroubleshootPage::icon() -> QIcon
{
	return Icon::get("exception");
}

auto TroubleshootPage::title() -> QString
{
	return QStringLiteral("Troubleshooting");
}

auto TroubleshootPage::save() -> bool
{
	return true;
}
