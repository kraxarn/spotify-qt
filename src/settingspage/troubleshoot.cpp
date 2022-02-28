#include "settingspage/troubleshoot.hpp"
#include "util/widget.hpp"

SettingsPage::Troubleshoot::Troubleshoot(lib::settings &settings,
	lib::cache &cache, QWidget *parent)
	: SettingsPage::Base(settings, parent),
	cache(cache)
{
	addTab(systemInfo(), QStringLiteral("System"));
	addTab(configPreview(), QStringLiteral("Config"));
	addTab(cacheInfo(), QStringLiteral("Cache"));
	addTab(developerMode(), QStringLiteral("Developer"));

	if (lib::crash_handler::is_init()
		|| !cache.get_all_crashes().empty())
	{
		addTab(crashLogs(), QStringLiteral("Crashes"));
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

auto SettingsPage::Troubleshoot::developerMode() -> QWidget *
{
	auto *layout = tabContent();

	auto *infoLabel = new QLabel(QStringLiteral(
		"Enable developer mode to show more options and to help troubleshooting issues. "
		"Developer mode is enabled until the application is closed."), this);

	infoLabel->setWordWrap(true);
	layout->addWidget(infoLabel);

	auto *devMode = new QCheckBox(QStringLiteral("Developer mode"), this);
	devMode->setChecked(lib::developer_mode::enabled);
	layout->addWidget(devMode);

	QCheckBox::connect(devMode, &QCheckBox::clicked,
		this, &SettingsPage::Troubleshoot::onDevModeClicked);

	return Widget::layoutToWidget(layout, this);
}

void SettingsPage::Troubleshoot::onDevModeClicked(bool checked)
{
	lib::developer_mode::enabled = checked;
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
