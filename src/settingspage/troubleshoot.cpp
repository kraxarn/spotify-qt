#include "settingspage/troubleshoot.hpp"

SettingsPage::Troubleshoot::Troubleshoot(lib::settings &settings,
	lib::cache &cache, QWidget *parent)
	: SettingsPage::Base(settings, parent),
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

auto SettingsPage::Troubleshoot::systemInfo() -> QWidget *
{
	return new SystemInfoView(this);
}

auto SettingsPage::Troubleshoot::configPreview() -> QWidget *
{
	return new ConfigView(settings, this);
}

auto SettingsPage::Troubleshoot::cacheInfo() -> QWidget *
{
	if (paths == nullptr)
	{
		paths = new QtPaths(this);
	}
	return new CacheView(*paths, this);
}

auto SettingsPage::Troubleshoot::crashLogs() -> QWidget *
{
	return new View::Crashes(cache, this);
}

auto SettingsPage::Troubleshoot::icon() -> QIcon
{
	return Icon::get("exception");
}

auto SettingsPage::Troubleshoot::title() -> QString
{
	return QStringLiteral("Troubleshooting");
}

auto SettingsPage::Troubleshoot::save() -> bool
{
	return true;
}
