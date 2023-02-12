#include "settingspage/about.hpp"
#include "mainwindow.hpp"
#include "widget/about.hpp"

SettingsPage::About::About(lib::settings &settings,
	const lib::http_client &httpClient,
	QWidget *parent)
	: SettingsPage::Base(settings, parent)
{
	addTab(about(), "General");
	addTab(contributors(httpClient), "Contributors");
}

auto SettingsPage::About::about() -> QWidget *
{
	return new ::About(this);
}

auto SettingsPage::About::contributors(const lib::http_client &httpClient) -> QWidget *
{
	return new List::Contributors(httpClient);
}

auto SettingsPage::About::icon() -> QIcon
{
	return Icon::get("help-about");
}

auto SettingsPage::About::title() -> QString
{
	return "About";
}

auto SettingsPage::About::save() -> bool
{
	return true;
}
